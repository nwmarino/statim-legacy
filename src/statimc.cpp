// Copyright 2024 Nick Marino (github.com/nwmarino)

/*
 * The main driver script to the statim compiler, hosted in C++.
 *
 * This source code is distributed as is, without any warranty.
 */

#include "llvm/IR/Module.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/TargetParser/Host.h"

#include <string>
#include <iostream>
#include <memory>
#include <utility>

#include "lexer.h"
#include "tstream.h"
#include "codegen.h"
#include "parser.h"
#include "ast.h"

using namespace llvm;
using namespace llvm::sys;

int main(int argc, char *argv[]) {
  std::string path("../test/function/main.statim");
  Lexer lex(path);

  std::shared_ptr<tstream> cc = lex.tokenize();
  cc->print();


  initializeModule();
  parse(cc);

  llvm::InitializeAllTargetInfos();
  llvm::InitializeAllTargets();
  llvm::InitializeAllTargetMCs();
  llvm::InitializeAllAsmParsers();
  llvm::InitializeAllAsmPrinters();

  auto TargetTriple = sys::getDefaultTargetTriple();
  TheModule->setTargetTriple(TargetTriple);


  modulePrint();

  return 0;
}
