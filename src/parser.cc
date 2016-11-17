#include <sstream>
#include <iostream>

#include "parser.hh"

Parser::Parser(std::string source) :
  lines(splitLines(source)),
  line(lines.begin()),
  column(line->begin())
{}

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

AST::OpType Parser::parseOperator() {
  skipWhitespace();

  if(*(column + 1) == '=') {
    switch (*column) {
      case '!':
        return AST::Neq;
      case '>':
        return AST::GtEq;
      case '<':
        return AST::LtEq;
      default:
        return AST::Invalid;
    }
  }

  switch (*column) {
    case '+':
      return AST::Add;
    case '-':
      return AST::Subtract;
    case '*':
      return AST::Multiply;
    case '/':
      return AST::Divide;
    case '%':
      return AST::Mod;
    case '=':
      return AST::Eq;
    case '>':
      return AST::Gt;
    case '<':
      return AST::Lt;
    default:
      return AST::Invalid;
  }
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
