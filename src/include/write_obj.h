/// Copyright 2024 Nick Marino (github.com/nwmarino)

#ifndef STATIMC_WRITE_OBJ_H
#define STATIMC_WRITE_OBJ_H

#include "llvm/Bitcode/BitcodeWriter.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Transforms/Utils/Cloning.h"

#include <string>

/**
 * Write an object file from an LLVM module.
 * 
 * @param LLModule The LLVM module to write to an object file.
 * @return         The name of the object file.
 */
std::string write_object_file(std::shared_ptr<llvm::Module> LLModule);

#endif  // STATIMC_WRITE_OBJ_H
