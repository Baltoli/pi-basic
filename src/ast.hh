#pragma once

#include <string>
#include <vector>

#include "llvm/IR/Value.h"

#include "Compiler.hh"

namespace AST {

struct Node {
  virtual llvm::Value *compile(Compiler::State &s) = 0;

  virtual ~Node() {};
};

struct Literal : public Node {
  int32_t value;

  Literal(int32_t v);
  
  llvm::Value *compile(Compiler::State &s) override;
};

struct BooleanLiteral : public Node {
  bool value;

  BooleanLiteral(bool v);

  llvm::Value *compile(Compiler::State &s) override;
};

struct Variable : public Node {
  std::string name;

  Variable(std::string n);

  llvm::Value *compile(Compiler::State &s) override;
};

enum BinaryOpType {
  Add,
  Subtract,
  Multiply,
  Divide,
  Mod,
  Eq,
  Neq,
  Gt,
  Lt,
  GtEq,
  LtEq,
  And,
  Or,
  Invalid
};

enum UnaryOpType {
  Not,
  UnaryInvalid
};

struct BinaryOp : public Node {
  Node *left;
  Node *right;
  BinaryOpType type;

  BinaryOp(Node *l, BinaryOpType t, Node *r);

  llvm::Value *compile(Compiler::State &s) override;
};

struct UnaryOp : public Node {
  Node *operand;
  UnaryOpType type;

  UnaryOp(UnaryOpType t, Node *op);

  llvm::Value *compile(Compiler::State &s) override;
};

struct Deref : public Node {
  Node *address;

  Deref(Node *a);

  llvm::Value *compile(Compiler::State &s) override;
};

struct Assign : public Node {
  Node *location;
  Node *value;

  Assign(Node *l, Node *v);

  llvm::Value *compile(Compiler::State &s) override;
};

struct WhileLoop : public Node {
  Node *condition;
  Node *body;

  WhileLoop(Node *c, Node *b);

  llvm::Value *compile(Compiler::State &s) override;
};

struct If : public Node {
  Node *condition;
  Node *trueBody;
  Node *falseBody;

  If(Node *c, Node *t);
  If(Node *c, Node *t, Node *f);

  llvm::Value *compile(Compiler::State &s) override;
};

struct Call : public Node {
  std::string name;
  std::vector<Node *> args;

  Call(std::string n, std::vector<Node *> a);

  llvm::Value *compile(Compiler::State &s) override;
};

struct Return : public Node {
  Node *value;

  Return(Node *v);

  llvm::Value *compile(Compiler::State &s) override;
};

struct FunctionDecl : public Node {
  std::string name;
  std::vector<std::string> params;
  Node *body;

  FunctionDecl(std::string n, std::vector<std::string> p, Node *b);

  llvm::Value *compile(Compiler::State &s) override;
};

struct FunctionList : public Node {
  std::vector<Node *> functions;

  FunctionList(std::vector<Node *> fs);

  llvm::Value *compile(Compiler::State &s) override;
};

struct StatementList : public Node {
  std::vector<Node *> statements;

  StatementList(std::vector<Node *> ss);

  llvm::Value *compile(Compiler::State &s) override;
};

struct Program : public Node {
  Node *functions;
  Node *body;

  Program(Node *fs, Node *b);

  llvm::Value *compile(Compiler::State &s) override;
};

}
