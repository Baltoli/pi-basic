#include "compiler.hh"
#include "ast.hh"

using namespace llvm;
using namespace AST;
using namespace Compiler;

Value *Literal::compile(State &s) {
  return nullptr;
}

Value *BooleanLiteral::compile(State &s) {
  return nullptr;
}

Value *Variable::compile(State &s) {
  return nullptr;
}

Value *BinaryOp::compile(State &s) {
  return nullptr;
}

Value *UnaryOp::compile(State &s) {
  return nullptr;
}

Value *Deref::compile(State &s) {
  return nullptr;
}

Value *Assign::compile(State &s) {
  return nullptr;
}

Value *WhileLoop::compile(State &s) {
  return nullptr;
}

Value *If::compile(State &s) {
  return nullptr;
}

Value *Call::compile(State &s) {
  return nullptr;
}

Value *FunctionDecl::compile(State &s) {
  return nullptr;
}

Value *FunctionList::compile(State &s) {
  return nullptr;
}

Value *StatementList::compile(State &s) {
  return nullptr;
}

Value *Program::compile(State &s) {
  return nullptr;
}
