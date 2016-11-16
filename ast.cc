#include "ast.hh"

using namespace llvm;

namespace AST {

Literal::Literal(int32_t v) : value(v) {}

Value *Literal::compile(Compiler::State &s) {
  return nullptr;
}

BooleanLiteral::BooleanLiteral(bool v) : value(v) {}

Value *BooleanLiteral::compile(Compiler::State &s) {
  return nullptr;
}

Variable::Variable(std::string n) : name(n) {}

Value *Variable::compile(Compiler::State &s) {
  return nullptr;
}

BinaryOp::BinaryOp(Node *l, OpType t, Node *r) :
  left(l), type(t), right(r) {}

Value *BinaryOp::compile(Compiler::State &s) {
  return nullptr;
}

Deref::Deref(Node *a) : address(a) {}

Value *Deref::compile(Compiler::State &s) {
  return nullptr;
}

Assign::Assign(std::string n, Node *v) : name(n), value(v) {}

Value *Assign::compile(Compiler::State &s) {
  return nullptr;
}

WhileLoop::WhileLoop(Node *c, Node *b) : condition(c), body(b) {}

Value *WhileLoop::compile(Compiler::State &s) {
  return nullptr;
}

If::If(Node *c, Node *t, Node* f) : condition(c), trueBody(t), falseBody(f) {}

Value *If::compile(Compiler::State &s) {
  return nullptr;
}

Call::Call(std::string n, std::vector<Node *> a) : name(n), args(a) {}

Value *Call::compile(Compiler::State &s) {
  return nullptr;
}

Function::Function(std::string n, std::vector<std::string> p, Node *b) :
  name(n), params(p), body(b) {}

Value *Function::compile(Compiler::State &s) {
  return nullptr;
}

FunctionList::FunctionList(std::vector<Node *> fs) : functions(fs) {}

Value *FunctionList::compile(Compiler::State &s) {
  return nullptr;
}

StatementList::StatementList(std::vector<Node *> ss) : statements(ss) {}

Value *StatementList::compile(Compiler::State &s) {
  return nullptr;
}

}
