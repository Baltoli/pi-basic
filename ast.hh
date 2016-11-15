#include "llvm/IR/Value.h"

struct ASTNode {
  virtual llvm::Value *compile() = 0;

  virtual ~ASTNode() {};
};

struct Literal : public ASTNode {
  int32_t value;

  Literal(int32_t v);
  
  llvm::Value *compile() override;
};
