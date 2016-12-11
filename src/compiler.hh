#ifndef COMPILER_HH
#define COMPILER_HH

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/DerivedTypes.h>

using namespace llvm;

namespace Compiler {

struct State {
  LLVMContext C;
  std::unique_ptr<Module> Mod;
  IRBuilder<> B;
  std::vector<Function> Funcs;
  IntegerType *intTy;

  Value *memory;

  Value *lookupSymbol(std::string name);
  void registerSymbol(std::string name, Value *val);
  void pushContext();
  void popContext();
  bool hasSymbol(std::string name);

  State();
private:
  std::vector<std::map<std::string, Value*>> symbols;
};

}

#endif
