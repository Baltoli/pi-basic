#ifndef COMPILER_HH
#define COMPILER_HH

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>

using namespace llvm;

namespace Compiler {

struct State {
  LLVMContext C;
  std::unique_ptr<Module> Mod;
  IRBuilder<> B;
  std::vector<Function> Funcs;

  State();
};

}

#endif
