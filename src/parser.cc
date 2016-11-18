#include <sstream>
#include <iostream>

#include "parser.hh"

Parser::Parser(std::string source) {
  lines = splitLines(source);
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

  if (isPrefix("true", column)) {
    column += 4;
    return new AST::BooleanLiteral(true);
  } else if (isPrefix("false", column)) {
    column += 5;
    return new AST::BooleanLiteral(false);
  }

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

  if(*(column + 1) == '=') {
    switch (*column) {
      case '!':
        column += 2;
        return AST::Neq;
      case '>':
        column += 2;
        return AST::GtEq;
      case '<':
        column += 2;
        return AST::LtEq;
      default:
        return AST::Invalid;
    }
  }

  switch (*column) {
    case '+':
      column += 1;
      return AST::Add;
    case '-':
      column += 1;
      return AST::Subtract;
    case '*':
      column += 1;
      return AST::Multiply;
    case '/':
      column += 1;
      return AST::Divide;
    case '%':
      column += 1;
      return AST::Mod;
    case '=':
      column += 1;
      return AST::Eq;
    case '>':
      column += 1;
      return AST::Gt;
    case '<':
      column += 1;
      return AST::Lt;
    default:
      if(isPrefix("and", column)) {
        column += 3;
        return AST::And;
      } else if(isPrefix("or", column)) {
        column += 2;
        return AST::Or;
      }
      return AST::Invalid;
  }
}

AST::UnaryOpType Parser::parseUnaryOperator() {
  skipWhitespace();

  if (isPrefix("not", column)) {
    column += 3;
    return AST::Not;
  }

  return AST::UnaryInvalid;
}

AST::Node *Parser::parseFactor() {
  skipWhitespace();

  auto maybeCall = parseCall();
  if(maybeCall) {
    return maybeCall;
  }

  auto maybeLiteral = parseLiteral();
  if(maybeLiteral) {
    return maybeLiteral;
  }

  auto maybeVariable = parseVariable();
  if(maybeVariable) {
    return maybeVariable;
  }

  auto prev = column;
  if (*column == '(') {
    column++;
    skipWhitespace();
    auto maybeExpr = parseExpression();
    skipWhitespace();
    if(*column == ')' && maybeExpr) {
      return maybeExpr;
    } else {
      column = prev;
    }
  }

  auto maybeDeref = parseDeref();
  if(maybeDeref) {
    return maybeDeref;
  }

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
  auto prev = column;
  skipWhitespace();

  if(*column != '[') {
    return nullptr;
  }

  column++;
  auto expr = parseExpression();
  if(!expr) {
    column = prev;
    return nullptr;
  }

  skipWhitespace();

  if(*column != ']') {
    column = prev;
    return nullptr;
  }

  column++;
  return new AST::Deref(expr);
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
  auto maybeCompare = parseComparison();
  if(maybeCompare) {
    return maybeCompare;
  }

  auto maybeLiteral = parseBooleanLiteral();
  if(maybeLiteral) {
    return maybeLiteral;
  }

  auto prev = column;
  if (*column == '(') {
    column++;
    skipWhitespace();
    auto maybeExpr = parseBoolean();
    skipWhitespace();
    if(*column == ')' && maybeExpr) {
      return maybeExpr;
    } else {
      column = prev;
    }
  }

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

void Parser::skipWhitespace() {
  while(!nonEmpty(*column)) {
    column++;
  }
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
