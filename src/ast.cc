#include "ast.hh"

using namespace llvm;

namespace AST {

Literal::Literal(int32_t v) : value(v) {}

BooleanLiteral::BooleanLiteral(bool v) : value(v) {}

Variable::Variable(std::string n) : name(n) {}

BinaryOp::BinaryOp(Node *l, BinaryOpType t, Node *r) :
  left(l), type(t), right(r) {}

UnaryOp::UnaryOp(UnaryOpType t, Node *op) :
  type(t), operand(op) {}

Deref::Deref(Node *a) : address(a) {}

Assign::Assign(Node *l, Node *v) : location(l), value(v) {}

WhileLoop::WhileLoop(Node *c, Node *b) : condition(c), body(b) {}

If::If(Node *c, Node *t, Node* f) : condition(c), trueBody(t), falseBody(f) {}
If::If(Node *c, Node *t) : condition(c), trueBody(t), falseBody(nullptr) {}

Call::Call(std::string n, std::vector<Node *> a) : name(n), args(a) {}

FunctionDecl::FunctionDecl(std::string n, std::vector<std::string> p, Node *b) :
  name(n), params(p), body(b) {}

FunctionList::FunctionList(std::vector<Node *> fs) : functions(fs) {}

StatementList::StatementList(std::vector<Node *> ss) : statements(ss) {}

Program::Program(Node *fs, Node *b) : functions(fs), body(b) {}

}
