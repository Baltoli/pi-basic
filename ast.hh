#pragma once

#include "llvm/IR/Value.h"

#include "compiler.hh"

struct ASTNode {
  virtual llvm::Value *compile(compiler::State *s) = 0;

  virtual ~ASTNode() {};
};

struct Literal : public ASTNode {
  int32_t value;

  Literal(int32_t v);
  
  llvm::Value *compile(compiler::State *s) override;
};
