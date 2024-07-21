// Copyright 2024 Nick Marino (github.com/nwmarino)

/*
 * The main driver script to the statim compiler, hosted in C++.
 *
 * This source code is distributed as is, without any warranty.
 */

#include <memory>
#include <string>

#include "include/codegen.h"
#include "include/container.h"
#include "include/lexer.h"
#include "include/parser.h"
#include "include/tstream.h"
#include "include/write_obj.h"

int main(int argc, char *argv[]) {
  std::string path = "./samples/integer/ret/ret_0.statim";
  Lexer lex(path);

  std::shared_ptr<tstream> cc = lex.tokenize();

  std::shared_ptr<LLContainer> container = std::make_shared<LLContainer>();
  initializeModule(container);
  parse(cc);
  
  std::string filename = write_object_file(container->getModule());
  system("clang++ output.o -o a");
  
  return EXIT_SUCCESS;
}
