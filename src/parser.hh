#pragma once

#include <string>
#include <vector>

#include "ast.hh"

using std::vector;
using std::string;

struct Parser {
  vector<string> lines;
  vector<string>::iterator line;
  string::iterator column;

  Parser(string source);

  AST::Node *parse();
  AST::Literal *parseLiteral();
  AST::BooleanLiteral *parseBooleanLiteral();
  AST::Variable *parseVariable();
  AST::BinaryOpType parseOperator();
  AST::Node *parseFactor();
  AST::Node *parseTerm();
  AST::Node *parseExpression();
  AST::Deref *parseDeref();
  AST::Call *parseCall();
  vector<AST::Node *> parseExpressionList();
  AST::BinaryOp *parseComparison();
  AST::UnaryOpType parseUnaryOperator();
  AST::Node *parseBooleanFactor();
  AST::Node *parseBooleanTerm();
  AST::Node *parseBoolean();
private:
  void skipWhitespace();

  static vector<string> splitLines(string source);
  static string &leftTrim(string &str);
  static string &rightTrim(string &str);
  static bool nonEmpty(char ch);
  static bool isPrefix(string pre, string::iterator source);
};
