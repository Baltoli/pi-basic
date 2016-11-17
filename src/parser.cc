#include <sstream>
#include <iostream>

#include "parser.hh"

Parser::Parser(std::string source) :
  lines(splitLines(source)),
  line(lines.begin()),
  column(line->begin())
{}

AST::Node *parse() {
  return nullptr;
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
