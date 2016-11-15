#pragma once

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

}
