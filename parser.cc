#include "parser.hh"

namespace Parser {

AST::Node *parse(std::string str) {
  auto it = std::string::iterator(str.begin());
  return parseNumericLiteral(it);
}

AST::Node *parseNumericLiteral(std::string::iterator &it) {
  int multiplier = (*it++ == '-') ? -1 : 1;
  return new AST::Literal(multiplier);
}

}
