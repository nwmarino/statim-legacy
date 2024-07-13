// Copyright 2024 Nick Marino (github.com/nwmarino)

/*
 * The main driver script to the statim compiler, hosted in C++.
 *
 * This source code is distributed as is, without any warranty.
 */

#include <string>
#include <iostream>
#include <memory>
#include <utility>

#include "lexer.h"
#include "tstream.h"
#include "codegen.h"
#include "parser.h"
#include "ast.h"

int main(int argc, char *argv[]) {
  std::string path("../test/function/main.statim");
  Lexer lex(path);

  std::shared_ptr<tstream> cc = lex.tokenize();
  cc->print();

  initializeModule();
  parse(cc);

  modulePrint();

  return 0;
}
