// Copyright 2024 Nick Marino (github.com/nwmarino)

/*
 * The main driver script to the statim compiler, hosted in C++.
 *
 * This source code is distributed as is, without any warranty.
 */

#include "llvm/Bitcode/BitcodeWriter.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/Transforms/Utils/Cloning.h"

#include <memory>
#include <string>

#include "codegen.h"
#include "container.h"
#include "lexer.h"
#include "parser.h"
#include "tstream.h"
#include "utils/write_obj.h"

int main(int argc, char *argv[]) {
  std::string path = "./samples/integer/ret/ret_0.statim";
  Lexer lex(path);

  std::shared_ptr<tstream> cc = lex.tokenize();

  std::shared_ptr<LLContainer> container = std::make_shared<LLContainer>();
  initializeModule(container);
  parse(cc);
  
  std::string filename = write_object_file(container->getModule());
  system("clang++ output.o -o a");
  //remove(filename);
  
  return EXIT_SUCCESS;
}
