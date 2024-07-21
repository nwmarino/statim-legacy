/// Copyright 2024 Nick Marino (github.com/nwmarino)

#ifndef STATIMC_CONTAINER_H
#define STATIMC_CONTAINER_H

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

#include <memory>

class LLContainer {
  std::shared_ptr<llvm::LLVMContext> LLContext;
  std::shared_ptr<llvm::Module> LLModule;
  std::shared_ptr<llvm::IRBuilder<>> Builder;

  public:
    /**
     * Construct a new container instance.
     */
    LLContainer();

    /**
     * Fetch the LLVM context.
     * @return Shared pointer to the LLVM context.
     */
    std::shared_ptr<llvm::LLVMContext> getContext();

    /**
     * Fetch the LLVM module.
     * @return Shared pointer to the LLVM module.
     */
    std::shared_ptr<llvm::Module> getModule();

    /**
     * Fetch the LLVM IR builder.
     * @return Shared pointer to the LLVM IR builder.
     */
    std::shared_ptr<llvm::IRBuilder<>> getBuilder();
};

#endif  // STATIMC_CONTAINER_H
