#include "ast.hh"

using namespace llvm;
using compiler::State;

Literal::Literal(int32_t v) : value(v) {}

Value *Literal::compile(State *s) {
  return nullptr;
}
