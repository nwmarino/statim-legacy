// Copyright 2024 Nick Marino (github.com/nwmarino)

/*
 * The main driver script to the statim compiler, hosted in C++.
 *
 * This source code is distributed as is, without any warranty.
 */

#include <llvm/IR/IRBuilder.h>
#include "llvm/IR/Module.h"
#include "llvm/IR/LegacyPassManager.h"
#include <llvm/IR/Verifier.h>
#include <llvm/IR/DataLayout.h>

#include "llvm/Target/TargetOptions.h"
#include "llvm/Target/TargetMachine.h"

#include "llvm/TargetParser/Host.h"

#include "llvm/Support/FileSystem.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"

#include "llvm/MC/TargetRegistry.h"

#include "llvm/ADT/STLExtras.h"

#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Utils/Cloning.h>

#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>

#include <llvm/Bitcode/BitcodeWriter.h>

#include <string>
#include <iostream>
#include <memory>
#include <utility>

#include "lexer.h"
#include "tstream.h"
#include "codegen.h"
#include "parser.h"
#include "ast.h"
#include "container.h"

using namespace llvm;
using namespace llvm::sys;

int main(int argc, char *argv[]) {
  std::string path = "./samples/ret_1.statim";
  Lexer lex(path);

  std::shared_ptr<tstream> cc = lex.tokenize();
  //cc->print();

  std::shared_ptr<LLContainer> container = std::make_shared<LLContainer>();
  initializeModule(container);
  std::shared_ptr<Module> LLModule = container->getModule();
  parse(cc);

  

  LLModule->print(errs(), nullptr);

  InitializeAllTargetInfos();
  InitializeAllTargets();
  InitializeAllTargetMCs();
  InitializeAllAsmParsers();
  InitializeAllAsmPrinters();
  
  auto TargetTriple = sys::getDefaultTargetTriple();

  std::string error;
  auto Target = TargetRegistry::lookupTarget(TargetTriple, error);

  if (!Target) {
    errs() << error;
    return 1;
  }

  auto CPU = "generic";
  auto Features = "";

  TargetOptions opt;
  auto TargetMachine = Target->createTargetMachine(
      TargetTriple, CPU, Features, opt, Reloc::PIC_
  );

  LLModule->setDataLayout(TargetMachine->createDataLayout());
  LLModule->setTargetTriple(TargetTriple);

  auto Filename = "output.o";
  std::error_code EC;
  raw_fd_ostream dest(Filename, EC, sys::fs::OF_None);

  if (EC) {
    errs() << "Could not open file: " << EC.message();
    return 1;
  }

  legacy::PassManager pass;
  auto FileType = CodeGenFileType::ObjectFile;

  if (TargetMachine->addPassesToEmitFile(pass, dest, nullptr, FileType)) {
    errs() << "TheTargetMachine can't emit a file of this type";
    return 1;
  }

  pass.run(*LLModule);
  dest.flush();

  return 0;
}
