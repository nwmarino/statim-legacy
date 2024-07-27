// Copyright 2024 Nick Marino (github.com/nwmarino)

/*
 * The main driver script to the statim compiler, hosted in C++.
 *
 * This source code is distributed as is, without any warranty.
 */

#include <memory>
#include <string>

#include "include/lexer.h"
#include "include/parser.h"
#include "include/tstream.h"

int main(int argc, char *argv[]) {
  std::string path = "./samples/integer/ret/ret_0.statim";
  Lexer lex(path);

  std::shared_ptr<tstream> cc = lex.tokenize();
  cc->print();

  parse(cc);
  
  return EXIT_SUCCESS;
}
