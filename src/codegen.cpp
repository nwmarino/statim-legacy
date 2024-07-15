// Copyright 2024 Nick Marino (github.com/nwmarino)

#include "ast.h"
#include "codegen.h"
#include "logger.h"
#include "container.h"

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IR/Value.h"

#include <memory>
#include <vector>
#include <map>

using namespace llvm;

static std::shared_ptr<LLVMContext> TheContext;
static std::shared_ptr<IRBuilder<>> Builder;
static std::shared_ptr<Module> TheModule;
static std::map<std::string, Value*> NamedValues;

Value *NumericalExpr::codegen()
{
  return ConstantFP::get(*TheContext, APFloat(value));
}

Value *VariableExpr::codegen()
{
  Value *val = NamedValues[name];
  if (!val)
    return logErrorV("Unresolved variable name.");
  return val;
}

Value *BinaryExpr::codegen()
{
  Value *L = leftSide->codegen();
  Value *R = rightSide->codegen();

  if (!L || !R)
    return nullptr;

  switch (op) {
    case '+':
      return Builder->CreateFAdd(L, R, "addtmp");
    case '-':
      return Builder->CreateFSub(L, R, "subtmp");
    case '*':
      return Builder->CreateFMul(L, R, "multmp");
    default:
      return logErrorV("Unresolved binary operator.");
  }
}

Value *FunctionCallExpr::codegen()
{
  Function *calleeF = TheModule->getFunction(callee);

  if (!calleeF)
    return logErrorV("Unresolved function reference.");

  if (calleeF->arg_size() != args.size())
    return logErrorV("Bad argument count");

  std::vector<Value *> argsV;
  for (unsigned i = 0, e = args.size(); i != e; ++i) {
    argsV.push_back(args[i]->codegen());
    if (!argsV.back())
      return nullptr;
  }

  return Builder->CreateCall(calleeF, argsV, "calltmp");
}

Function *PrototypeAST::codegen()
{
  std::vector<Type *> Doubles(args.size(), Type::getDoubleTy(*TheContext));
  FunctionType *FT = FunctionType::get(Type::getDoubleTy(*TheContext), Doubles, false);
  Function *F = Function::Create(FT, Function::ExternalLinkage, name, TheModule.get()); 

  unsigned idx = 0;
  for (auto &arg : F->args())
    arg.setName(args[idx++]);

  return F;
}

Function *FunctionAST::codegen()
{
  Function *TheFunction = TheModule->getFunction(head->getName());

  if (!TheFunction)
    TheFunction = head->codegen();

  if (!TheFunction)
    return nullptr;

  if (!TheFunction->empty())
    return (Function *)logErrorV("Function cannot be redefined.");

  BasicBlock *BB = BasicBlock::Create(*TheContext, "entry", TheFunction);
  Builder->SetInsertPoint(BB);

  NamedValues.clear();
  for (auto &arg : TheFunction->args())
    NamedValues[std::string(arg.getName())] = &arg;

  if (Value *V = body->codegen()) {
    verifyFunction(*TheFunction);
    return TheFunction;
  }
  
  TheFunction->eraseFromParent();
  return nullptr;
}

Value *ReturnStatement::codegen()
{
  if (Value *retVal = expr->codegen())
    return Builder->CreateRet(retVal);
  return nullptr;
}

void initializeModule(std::shared_ptr<LLContainer> container)
{
  TheContext = container->getContext();
  TheModule = container->getModule();
  Builder = container->getBuilder();
}

void modulePrint()
{
  TheModule->print(errs(), nullptr);
}
