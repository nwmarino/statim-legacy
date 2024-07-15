/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

#include <memory>

#include "container.h"

using namespace llvm;

LLContainer::LLContainer()
{
  this->LLContext = std::make_shared<LLVMContext>();
  this->LLModule = std::make_shared<Module>("STATIM_C", *LLContext);
  this->Builder = std::make_shared<IRBuilder<>>(*LLContext);
}

std::shared_ptr<LLVMContext> LLContainer::getContext()
{
  return LLContext;
}

std::shared_ptr<Module> LLContainer::getModule()
{
  return LLModule;
}

std::shared_ptr<IRBuilder<>> LLContainer::getBuilder()
{
  return Builder;
}
