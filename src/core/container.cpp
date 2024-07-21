/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

#include <memory>

#include "../include/container.h"

/**
 * Construct a new container instance.
 */
LLContainer::LLContainer()
{
  this->LLContext = std::make_shared<llvm::LLVMContext>();
  this->LLModule = std::make_shared<llvm::Module>("STATIM_C", *LLContext);
  this->Builder = std::make_shared<llvm::IRBuilder<>>(*LLContext);
}

/**
 * Fetch the LLVM context.
 * @return Shared pointer to the LLVM context.
 */
std::shared_ptr<llvm::LLVMContext> LLContainer::getContext() {
  return LLContext;
}

/**
 * Fetch the LLVM module.
 * @return Shared pointer to the LLVM module.
 */
std::shared_ptr<llvm::Module> LLContainer::getModule() {
  return LLModule;
}

/**
 * Fetch the LLVM IR builder.
 * @return Shared pointer to the LLVM IR builder.
 */
std::shared_ptr<llvm::IRBuilder<>> LLContainer::getBuilder() {
  return Builder;
}
