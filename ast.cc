#include "ast.hh"

using namespace llvm;

Literal::Literal(int32_t v) : value(v) {}

Value *Literal::compile() {
  return nullptr;
}
