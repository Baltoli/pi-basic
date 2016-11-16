#pragma once

#include <string>

#include "llvm/IR/Value.h"

#include "Compiler.hh"

namespace AST {

struct Node {
  virtual llvm::Value *compile(Compiler::State *s) = 0;

  virtual ~Node() {};
};

struct Literal : public Node {
  int32_t value;

  Literal(int32_t v);
  
  llvm::Value *compile(Compiler::State *s) override;
};

struct BooleanLiteral : public Node {
  bool value;

  BooleanLiteral(bool v);

  llvm::Value *compile(Compiler::State *s) override;
};

struct Variable : public Node {
  std::string name;

  Variable(std::string n);

  llvm::Value *compile(Compiler::State *s) override;
};

enum OpType {
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
  LtEq
};

struct BinaryOp : public Node {
  Node *left;
  Node *right;
  OpType type;

  BinaryOp(Node *l, OpType t, Node *r);

  llvm::Value *compile(Compiler::State *s) override;
};

struct Deref : public Node {
  Node *address;

  Deref(Node *a);

  llvm::Value *compile(Compiler::State *s) override;
};

}
