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
private:
  static vector<string> splitLines(string source);
  static string &leftTrim(string &str);
  static string &rightTrim(string &str);
  static bool nonEmpty(char ch);
};
