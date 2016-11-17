#include <iostream>

#include "ast.hh"
#include "parser.hh"

int main() {
  auto x = Parser::parse("123");
  std::cout << dynamic_cast<AST::Literal *>(x)->value << std::endl;
  return 0;
}
