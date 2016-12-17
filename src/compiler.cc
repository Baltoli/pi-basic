#include <memory>

#include <llvm/IR/Constants.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Function.h>

#include "compiler.hh"
#include "ast.hh"

using namespace llvm;
using namespace AST;
using namespace Compiler;

State::State() : B(C) {
  intTy = IntegerType::get(C, 32);
  Mod = std::unique_ptr<Module>(new Module("main-mod", C));  

  Type *memTy = ArrayType::get(intTy, 1024);
  memory = new GlobalVariable(*Mod, memTy, false, GlobalValue::CommonLinkage, 0, "memory");
}

Value *State::lookupSymbol(std::string name) {
  for(auto it = symbols.rbegin(); it != symbols.rend(); it++) {
    auto map = *it;
    if(map.find(name) != map.end()) {
      return map[name];
    }
  }

  return nullptr;
}

void State::registerSymbol(std::string name, Value *val) {
  if(symbols.empty()) {
    return;
  }

  symbols.back()[name] = val;
}

void State::pushContext() {
  symbols.push_back(std::map<std::string, Value*>());
}

void State::popContext() {
  symbols.pop_back();
}

bool State::hasSymbol(std::string name) {
  for(auto it = symbols.rbegin(); it != symbols.rend(); it++) {
    auto map = *it;
    if(map.find(name) != map.end()) {
      return true;
    }
  }

  return false;
}

Value *Literal::compile(State &s) {
  return ConstantInt::getSigned(s.intTy, value);
}

Value *BooleanLiteral::compile(State &s) {
  return value ? ConstantInt::getTrue(s.intTy) : ConstantInt::getFalse(s.intTy);
}

Value *Variable::compile(State &s) {
  return s.lookupSymbol(name);
}

Value *BinaryOp::compile(State &s) {
  Value *lhs = left->compile(s);
  Value *rhs = right->compile(s);

  switch(type) {
    case Add:
      return s.B.CreateAdd(lhs, rhs);
    case Subtract:
      return s.B.CreateSub(lhs, rhs);
    case Multiply:
      return s.B.CreateMul(lhs, rhs);
    case Divide:
      return s.B.CreateSDiv(lhs, rhs);
    case Mod:
      return s.B.CreateSRem(lhs, rhs);
    case Eq:
      return s.B.CreateICmpEQ(lhs, rhs);
    case Neq:
      return s.B.CreateICmpNE(lhs, rhs);
    case Gt:
      return s.B.CreateICmpSGT(lhs, rhs);
    case Lt:
      return s.B.CreateICmpSLT(lhs, rhs);
    case GtEq:
      return s.B.CreateICmpSGE(lhs, rhs);
    case LtEq:
      return s.B.CreateICmpSLE(lhs, rhs);
    case And:
      return s.B.CreateAnd(lhs, rhs);
    case Or:
      return s.B.CreateOr(lhs, rhs);
    default:
      return nullptr;
  }
}

Value *UnaryOp::compile(State &s) {
  Value *v = operand->compile(s);

  switch(type) {
    case Not:
      return s.B.CreateNot(v);
    default:
      return nullptr;
  }
}

Value *Deref::compile(State &s) {
  Value *addr = address->compile(s);
  Type *ptrTy = PointerType::getUnqual(s.intTy);
  Value *ptr = s.B.CreateBitCast(s.memory, ptrTy);
  Value *offset = s.B.CreateGEP(ptr, addr);
  return s.B.CreateLoad(offset);
}

Value *Assign::compile(State &s) {
  auto deref = dynamic_cast<Deref *>(location);
  Value *loc;
  if(deref) {
    Value *offset = deref->address->compile(s);
    Type *ptrTy = PointerType::getUnqual(s.intTy);
    Value *ptr = s.B.CreateBitCast(s.memory, ptrTy);
    loc = s.B.CreateGEP(ptr, offset);
  }

  auto var = dynamic_cast<Variable *>(location);
  if(var) {
    if(!s.hasSymbol(var->name)) {
      s.registerSymbol(var->name, s.B.CreateAlloca(s.intTy));
    }
    loc = s.lookupSymbol(var->name);
  }

  auto val = value->compile(s);
  return s.B.CreateStore(val, loc);
}

Value *WhileLoop::compile(State &s) {
  return nullptr;
}

Value *If::compile(State &s) {
  return nullptr;
}

Value *Call::compile(State &s) {
  std::vector<Value *> argVals;

  for(auto arg : args) {
    argVals.push_back(arg->compile(s));
  }

  Function *f = s.Mod->getFunction(name);

  return s.B.CreateCall(f, argVals);
}

Value *FunctionDecl::compile(State &s) {
  Type *retTy = s.intTy;
  std::vector<Type *> paramTypes;
  for(auto p : params) {
    paramTypes.push_back(s.intTy);
  }

  FunctionType *funcTy = FunctionType::get(retTy, paramTypes, false);
  Function *f = Function::Create(funcTy, GlobalValue::ExternalLinkage, name, s.Mod.get());

  s.pushContext();

  int i = 0;
  for(auto it = f->arg_begin(); it != f->arg_end(); it++, i++) {
    s.registerSymbol(params[i], &(*it));
  }

  BasicBlock *entry = BasicBlock::Create(s.C, "entry", f);
  s.B.SetInsertPoint(entry);
  Value *last = body->compile(s);

  s.popContext();
  
  if(isa<ReturnInst>(last)) {
    return last;
  }

  return s.B.CreateRetVoid();
}

Value *Return::compile(State &s) {
  return s.B.CreateRet(value->compile(s));
}

Value *FunctionList::compile(State &s) {
  for(auto func : functions) {
    func->compile(s);
  }

  return nullptr;
}

Value *StatementList::compile(State &s) {
  Value *last = nullptr;
  for(auto stmt : statements) {
    last = stmt->compile(s);
  }

  return last;
}

Value *Program::compile(State &s) {
  functions->compile(s);

  s.pushContext();

  Type *retTy = IntegerType::get(s.C, 32);
  Type *argcTy = IntegerType::get(s.C, 32);
  Type *argvTy = PointerType::getUnqual(PointerType::getUnqual(IntegerType::get(s.C, 8)));

  Function *f = Function::Create(
      FunctionType::get(retTy, { argcTy, argvTy }, false),
      GlobalValue::ExternalLinkage,
      "main",
      s.Mod.get());
  BasicBlock *main = BasicBlock::Create(s.C, "entry", f);
  s.B.SetInsertPoint(main);
  body->compile(s);

  s.Mod->dump();
  return nullptr;
}
