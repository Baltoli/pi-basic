#include <sstream>
#include <iostream>
#include <map>

#include "parser.hh"

using std::experimental::optional;

#define OPTION(parser) \
  do { \
    auto ret = parse##parser(); \
    if(ret) { \
      return ret; \
    } \
  } while(false) 

#define OPTION_MAP(f, parser) \
  do { \
    auto ret = f(parser); \
    if(ret) { \
      return ret; \
    } \
  } while(false)

#define PARENS(parser) \
  (wrap("(", &Parser::parse##parser, ")"))

#define KEYWORD(kw, ret) \
  do { \
    if(keyword(kw)) { \
      return ret; \
    } \
  } while(false);

Parser::Parser(std::string source) {
  lines = splitLines(source);
  errors = {};
  if(lines.size() > 0) {
    line = lines.begin();
    column = line->begin();
  } else {
    // FIXME: better handling of errors in the constructor. Throwing here is a
    // hack and should be fixed in the future - though we probably do want to
    // throw *something* if initialising with an empty string.
    throw;
  }
}

AST::Node *Parser::parse() {
  return nullptr;
}

AST::Literal *Parser::parseLiteral() {
  skipWhitespace();

  auto base = column;

  int multiplier = (*column == '-') ? -1 : 1;
  if(multiplier == -1) {
    column++;
  }

  std::ostringstream stream;
  while(std::isdigit(*column)) {
    stream << *column++;
  }

  string ret = stream.str();
  if(ret != "") {
    return new AST::Literal(multiplier * strtol(ret.c_str(), NULL, 10));
  }

  column = base;
  return nullptr;
}

AST::BooleanLiteral *Parser::parseBooleanLiteral() {
  skipWhitespace();

  KEYWORD("true", new AST::BooleanLiteral(true));
  KEYWORD("false", new AST::BooleanLiteral(false));

  return nullptr;
}

AST::Variable *Parser::parseVariable() {
  skipWhitespace();

  if(!(std::isalpha(*column) || *column == '_')) {
    return nullptr;
  }

  std::ostringstream stream;
  while(std::isalnum(*column) || *column == '_') {
    stream << *column++;
  }

  if (stream.str() != "") {
    return new AST::Variable(stream.str());
  }

  return nullptr;
}

AST::BinaryOpType Parser::parseOperator() {
  skipWhitespace();

  std::map<string, AST::BinaryOpType> ops = {
    { "+", AST::Add },
    { "-", AST::Subtract },
    { "*", AST::Multiply },
    { "/", AST::Divide },
    { "%", AST::Mod },
    { "!=", AST::Neq },
    { "=", AST::Eq },
    { ">=", AST::GtEq },
    { "<=", AST::LtEq },
    { ">", AST::Gt },
    { "<", AST::Lt },
    { "and", AST::And },
    { "or", AST::Or }
  };

  optional<string> op = longestKeyword(ops);
  if(op) {
    keyword(*op);
    return(ops[*op]);
  }

  return AST::Invalid;
}

AST::UnaryOpType Parser::parseUnaryOperator() {
  skipWhitespace();

  KEYWORD("not", AST::Not);

  return AST::UnaryInvalid;
}

AST::Node *Parser::parseFactor() {
  skipWhitespace();

  OPTION(Call);
  OPTION(Literal);
  OPTION(Variable);
  OPTION_MAP(PARENS, Expression);
  OPTION(Deref);

  return nullptr;
}

AST::Node *Parser::parseTerm() {
  skipWhitespace();

  auto first = parseFactor();
  auto prev = column;
  auto op = parseOperator();

  if(op != AST::Invalid) {
    auto second = parseFactor();
    auto opValid = (op == AST::Multiply) || (op == AST::Divide) || (op == AST::Mod);

    if(first && second && opValid) {
      return new AST::BinaryOp(first, op, second);
    }
  }

  column = prev;
  return first;
}

AST::Node *Parser::parseExpression() {
  skipWhitespace();

  auto first = parseTerm();
  auto prev = column;
  auto op = parseOperator();

  if(op != AST::Invalid) {
    auto second = parseTerm();
    auto opValid = (op == AST::Add) || (op == AST::Subtract);

    if(first && second && opValid) {
      return new AST::BinaryOp(first, op, second);
    }
  }

  column = prev;
  return first;
}

AST::Deref *Parser::parseDeref() {
  auto dr = wrap("[", &Parser::parseExpression, "]");
  if(dr) {
    return new AST::Deref(dr);
  }

  return nullptr;
}

AST::Call *Parser::parseCall() {
  auto prev = column;

  AST::Variable *maybeV = parseVariable();
  if(maybeV == nullptr) {
    return nullptr;
  }

  if(*column != '(') {
    column = prev;
    return nullptr;
  }

  column++;
  auto exprs = parseExpressionList();
  skipWhitespace();

  if(*column != ')') {
    column = prev;
    return nullptr;
  }

  column++;
  return new AST::Call(maybeV->name, exprs);
}

vector<AST::Node *> Parser::parseExpressionList() {
  vector<AST::Node *> vec;
  AST::Node *expr;

  bool comma = false;
  while(true) {
    expr = parseExpression();
    if(expr != nullptr) {
      vec.push_back(expr);
    } else if(comma) {
      column--;
      break;
    }

    comma = false;
    skipWhitespace();

    if(*column == ',') {
      comma = true;
      column++;
    } else {
      break;
    }
  }

  return vec;
}

vector<string> Parser::parseArgumentList() {
  vector<string> vec;
  AST::Variable *expr;

  bool comma = false;
  while(true) {
    expr = parseVariable();
    if(expr != nullptr) {
      vec.push_back(expr->name);
    } else if(comma) {
      column--;
      break;
    }

    comma = false;
    skipWhitespace();

    if(*column == ',') {
      comma = true;
      column++;
    } else {
      break;
    }
  }

  return vec;
}

AST::BinaryOp *Parser::parseComparison() {
  auto prev = column;

  auto left = parseExpression();
  auto op = parseOperator();
  auto right = parseExpression();

  bool validOp = 
    (op == AST::Eq) || (op == AST::Neq) ||
    (op == AST::Lt) || (op == AST::Gt) ||
    (op == AST::LtEq) || (op == AST::GtEq);

  if(left && right && validOp) {
    return new AST::BinaryOp(left, op, right);
  }

  column = prev;
  return nullptr;
}

AST::Node *Parser::parseBooleanFactor() {
  OPTION(Comparison);
  OPTION(BooleanLiteral);
  OPTION_MAP(PARENS, Boolean);

  auto op = parseUnaryOperator();
  auto maybeBool = parseBoolean();
  if(op != AST::UnaryInvalid && maybeBool) {
    return new AST::UnaryOp(op, maybeBool);
  }

  return nullptr;
}

AST::Node *Parser::parseBooleanTerm() {
  skipWhitespace();

  auto first = parseBooleanFactor();
  auto prev = column;
  auto op = parseOperator();

  if(op != AST::Invalid) {
    auto second = parseBooleanFactor();
    auto opValid = (op == AST::And);

    if(first && second && opValid) {
      return new AST::BinaryOp(first, op, second);
    }
  }

  column = prev;
  return first;
}

AST::Node *Parser::parseBoolean() {
  skipWhitespace();

  auto first = parseBooleanTerm();
  auto prev = column;
  auto op = parseOperator();

  if(op != AST::Invalid) {
    auto second = parseBooleanTerm();
    auto opValid = (op == AST::Or);

    if(first && second && opValid) {
      return new AST::BinaryOp(first, op, second);
    }
  }

  column = prev;
  return first;
}

AST::Assign *Parser::parseAssign() {
  auto prev = column;
  skipWhitespace();

  AST::Node *loc = parseVariable();
  if(loc == nullptr) {
    column = prev;
    loc = parseDeref();
  }

  if(loc == nullptr) {
    column = prev;
    return nullptr;
  }

  skipWhitespace();

  if(!isPrefix("<-", column)) {
    column = prev;
    return nullptr;
  }

  column += 2;
  skipWhitespace();

  auto maybeExpr = parseExpression();
  if(maybeExpr) {
    return new AST::Assign(loc, maybeExpr);
  }

  column = prev;
  return nullptr;
}

AST::Node *Parser::parseStatement() {
  OPTION(Assign);
  OPTION(If);
  OPTION(WhileLoop);

  return nullptr;
}

AST::If *Parser::parseIf() {
  skipWhitespace();
  auto prev = column;

  if(keyword("if")) {
    auto maybeB = parseBoolean();
    if(nextLine() && maybeB) {
      auto list = parseStatementList();
      if(list && keyword("end")) {
        return new AST::If(maybeB, list, nullptr);
      } else if(list && keyword("else") && nextLine()) {
        auto falseList = parseStatementList();
        if(falseList && keyword("end")) {
          return new AST::If(maybeB, list, falseList);
        }
      }
    }
  }

  column = prev;
  return nullptr;
}

AST::WhileLoop *Parser::parseWhileLoop() {
  skipWhitespace();
  auto prev = column;

  if(keyword("while")) {
    auto maybeB = parseBoolean();
    if(nextLine() && maybeB) {
      auto list = parseStatementList();
      if(list && keyword("end")) {
        return new AST::WhileLoop(maybeB, list);
      }
    }
  }

  column = prev;
  return nullptr;
}

AST::StatementList *Parser::parseStatementList() {
  vector<AST::Node *> results;
  AST::Node *match;

  while((match = matchLine(&Parser::parseStatement))) {
    results.push_back(match);
  }

  return new AST::StatementList(results);
}

AST::FunctionDecl *Parser::parseFunctionDeclaration() {
  auto prev = column;
  skipWhitespace();

  if(keyword("function ")) {
    skipWhitespace();
    auto maybeV = parseVariable();
    if(maybeV && keyword("(")) {
      auto list = parseArgumentList();
      if(keyword(")") && nextLine()) {
        auto stmts = parseStatementList();
        if(stmts && keyword("end")) {
          return new AST::FunctionDecl(maybeV->name, list, stmts);
        }
      }
    }
  }

  column = prev;
  return nullptr;
}

AST::FunctionList *Parser::parseFunctionList() {
  vector<AST::Node *> results;
  AST::Node *match;

  while((match = parseFunctionDeclaration())) {
    nextLine();
    skipLines();
    results.push_back(match);
  }

  return new AST::FunctionList(results);
}

AST::Program *Parser::parseProgram() {
  auto functions = parseFunctionList();
  auto statements = parseStatementList();
  line++;

  if(line != lines.end()) {
    error("Failed to parse whole file");
  }

  if(errors.size() > 0) {
    return nullptr;
  }

  return new AST::Program(functions, statements);
}

template<typename T>
optional<string> Parser::longestKeyword(std::map<string, T> kws) {
  size_t max = 0;
  string maxVal;

  for(const auto& kw : kws) {
    size_t length = keyword(kw.first, false);
    if(length > max) {
      max = length;
      maxVal = kw.first;
    }
  }

  optional<string> empty {};
  return max > 0 ? maxVal : empty;
}

size_t Parser::keyword(string kw, bool eat) {
  skipWhitespace();
  if(isPrefix(kw, column)) {
    if(eat) { column += kw.length(); }

    return kw.length();
  }
  return 0;
}

template<typename T> 
T *Parser::wrap(string left, T *(Parser::* p)(), string right) {
  auto prev = column;
  skipWhitespace();

  if(keyword(left)) {
    skipWhitespace();
    auto maybeT = (this->*p)();
    skipWhitespace();
    if(keyword(right) && maybeT) {
      return maybeT;
    }
  }

  column = prev;
  return nullptr;
}

template<typename T>
T *Parser::matchLine(T *(Parser::* p)()) {
  auto prev = column;
  skipWhitespace();

  T *maybeT;
  if((maybeT = (this->*p)())) {
    if(column == line->end()) {
      nextLine();
      return maybeT;
    }
  }

  column = prev;
  return nullptr;
}

bool Parser::nextLine() {
  if(line != lines.end() - 1) {
    line++;
    column = line->begin();
    return true;
  }

  return false;
}

void Parser::skipWhitespace() {
  while(!nonEmpty(*column)) {
    column++;
  }
}

void Parser::error(string e) {
  errors.push_back(e);
}

vector<string> Parser::splitLines(string source) {
  vector<string> ret;
  std::stringstream stream(source);

  string item;
  while(std::getline(stream, item)) {
    item = leftTrim(rightTrim(item));
    if(item != "") {
      ret.push_back(item);
    }
  }

  return ret;
}

string &Parser::leftTrim(string &str) {
  auto it = std::find_if(str.begin(), str.end(), nonEmpty);
  str.erase(str.begin(), it);
  return str;
}

string &Parser::rightTrim(string &str) {
  auto it = std::find_if(str.rbegin(), str.rend(), nonEmpty);
  str.erase(it.base(), str.end());
  return str;
}

bool Parser::nonEmpty(char ch) {
  return !std::isspace<char>(ch , std::locale::classic());
}

bool Parser::isPrefix(string pre, string::iterator source) {
  return std::equal(pre.begin(), pre.end(), source);
}

void Parser::skipLines() {
  while(line->begin() == line->end()) {
    nextLine();
  }
}
