#include "ast.hh"

using namespace llvm;

namespace AST {

Literal::Literal(int32_t v) : value(v) {}

Value *Literal::compile(Compiler::State *s) {
  return nullptr;
}

BooleanLiteral::BooleanLiteral(bool v) : value(v) {}

Value *BooleanLiteral::compile(Compiler::State *s) {
  return nullptr;
}

Variable::Variable(std::string n) : name(n) {}

Value *Variable::compile(Compiler::State *s) {
  return nullptr;
}

}
