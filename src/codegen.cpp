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

Value *Numerical::codegen() {
  return ConstantFP::get(*TheContext, APFloat(value));
}

Value *Variable::codegen() {
  Value *val = NamedValues[name];
  if (!val)
    logErrorV("Unresolved variable name.");
  return val;
}

