// Copyright (c) 2024 Nick Marino (github.com/nwmarino)

#include "codegen.h"
#include "ast.h"
#include "logger.h"

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
#include <map>

using namespace llvm;

static std::unique_ptr<LLVMContext> TheContext;
static std::unique_ptr<IRBuilder<>> Builder(TheContext);
static std::unique_ptr<Module> TheModule;
static std::map<std::string, Value*> NamedValues;

Value *NumericalExpr::codegen() {
  return ConstantFP::get(*TheContext, APFloat(value));
}

Value *VariableExpr::codegen() {
  Value *val = NamedValues[name];
  if (!val)
    logErrorV("Unresolved variable name.");
  return val;
}

Value *BinaryExpr::codegen() {
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

Value *FunctionCallExpr::codegen() {
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
