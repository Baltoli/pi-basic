#pragma once

#include <string>
#include <vector>
#include <experimental/optional>
#include <map>

#include "ast.hh"

using std::vector;
using std::string;
using std::map;
using std::experimental::optional;

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
  vector<string> parseArgumentList();
  AST::BinaryOp *parseComparison();
  AST::UnaryOpType parseUnaryOperator();
  AST::Node *parseBooleanFactor();
  AST::Node *parseBooleanTerm();
  AST::Node *parseBoolean();
  AST::Assign *parseAssign();
  AST::Node *parseStatement();
  AST::If *parseIf();
  AST::WhileLoop *parseWhileLoop();
  AST::StatementList *parseStatementList();
  AST::FunctionDecl *parseFunctionDeclaration();
  AST::FunctionList *parseFunctionList();
private:
  void skipWhitespace();
  bool nextLine();
  size_t keyword(string kw, bool eat = true);
  template<typename T> optional<string> longestKeyword(map<string, T> kws);
  template<typename T> T *wrap(string left, T *(Parser::*)(), string right);
  template<typename T> T *matchLine(T *(Parser::* p)());

  static vector<string> splitLines(string source);
  static string &leftTrim(string &str);
  static string &rightTrim(string &str);
  static bool nonEmpty(char ch);
  static bool isPrefix(string pre, string::iterator source);
};
