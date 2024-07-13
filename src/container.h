// Copyright 2024 Nick Marino (github.com/nwmarino)

#ifndef CONTAINER_H
#define CONTAINER_H

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"

#include <memory>

using namespace llvm;

class LLContainer {
  std::unique_ptr<LLVMContext> LLContext;
  std::unique_ptr<Module> LLModule;
  std::unique_ptr<IRBuilder<>> Builder;


  public:
    LLContainer();
}

#endif  // CONTAINER_H