// Copyright (c) 2024 Nick Marino (github.com/nwmarino)

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

  delete &cc;

  modulePrint();

  return 0;
}
