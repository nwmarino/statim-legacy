// Copyright 2024 Nick Marino (github.com/nwmarino)

#ifndef CONTAINER_H
#define CONTAINER_H

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

#include <memory>

using namespace llvm;

class LLContainer {
  std::shared_ptr<llvm::LLVMContext> LLContext;
  std::shared_ptr<Module> LLModule;
  std::shared_ptr<IRBuilder<>> Builder;

  public:
    LLContainer();
    std::shared_ptr<llvm::LLVMContext> getContext();
    std::shared_ptr<Module> getModule();
    std::shared_ptr<IRBuilder<>> getBuilder();
};

#endif  // CONTAINER_H