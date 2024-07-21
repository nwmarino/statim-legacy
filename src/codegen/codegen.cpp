/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include "llvm/ADT/APInt.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IR/Value.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "../include/ast.h"
#include "../include/container.h"
#include "../include/logger.h"

static std::map<std::string, llvm::Value*> NamedValues;

llvm::Value *IntegerExpr::codegen(std::shared_ptr<LLContainer> container) {
  return llvm::ConstantInt::get(llvm::Type::getInt32Ty(*container->getContext()), value);
}


llvm::Value *FloatingPointExpr::codegen(std::shared_ptr<LLContainer> container) {
  return llvm::ConstantFP::get(*container->getContext(), llvm::APFloat(value));
}


llvm::Value *VariableExpr::codegen(std::shared_ptr<LLContainer> container) {
  llvm::Value *val = NamedValues[name];
  if (!val)
    return logErrorV("Unresolved variable name.");
  return val;
}


llvm::Value *BinaryExpr::codegen(std::shared_ptr<LLContainer> container) {
  llvm::Value *L = leftSide->codegen(container);
  llvm::Value *R = rightSide->codegen(container);

  if (!L || !R)
    return nullptr;

  switch (op) {
    case '+':
      return container->getBuilder()->CreateFAdd(L, R, "addtmp");
    case '-':
      return container->getBuilder()->CreateFSub(L, R, "subtmp");
    case '*':
      return container->getBuilder()->CreateFMul(L, R, "multmp");
    default:
      return logErrorV("Unresolved binary operator.");
  }
}


llvm::Value *FunctionCallExpr::codegen(std::shared_ptr<LLContainer> container) {
  llvm::Function *calleeF = container->getModule()->getFunction(callee);

  if (!calleeF)
    return logErrorV("Unresolved function reference.");

  if (calleeF->arg_size() != args.size())
    return logErrorV("Bad argument count");

  std::vector<llvm::Value *> argsV;
  for (unsigned i = 0, e = args.size(); i != e; ++i) {
    argsV.push_back(args[i]->codegen(container));
    if (!argsV.back())
      return nullptr;
  }

  return container->getBuilder()->CreateCall(calleeF, argsV, "calltmp");
}


llvm::Function *PrototypeAST::codegen(std::shared_ptr<LLContainer> container) {
  std::vector<llvm::Type *> Doubles(args.size(), llvm::Type::getDoubleTy(*container->getContext()));
  llvm::FunctionType *FT = llvm::FunctionType::get(llvm::Type::getInt32Ty(*container->getContext()), Doubles, false);
  llvm::Function *F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, name, container->getModule().get()); 

  unsigned idx = 0;
  for (auto &arg : F->args())
    arg.setName(args[idx++]);

  return F;
}


llvm::Function *FunctionAST::codegen(std::shared_ptr<LLContainer> container) {
  llvm::Function *TheFunction = container->getModule()->getFunction(head->getName());

  if (!TheFunction)
    TheFunction = head->codegen(container);

  if (!TheFunction)
    return nullptr;

  if (!TheFunction->empty())
    return (llvm::Function *)logErrorV("Function cannot be redefined.");

  llvm::BasicBlock *BB = llvm::BasicBlock::Create(*container->getContext(), "entry", TheFunction);
  container->getBuilder()->SetInsertPoint(BB);

  NamedValues.clear();
  for (auto &arg : TheFunction->args())
    NamedValues[std::string(arg.getName())] = &arg;

  if (llvm::Value *V = body->codegen(container)) {
    verifyFunction(*TheFunction);
    return TheFunction;
  }
  
  TheFunction->eraseFromParent();
  return nullptr;
}


llvm::Value *ReturnStatement::codegen(std::shared_ptr<LLContainer> container) {
  if (llvm::Value *retVal = expr->codegen(container))
    return container->getBuilder()->CreateRet(retVal);
  return nullptr;
}
