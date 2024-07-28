/// Copyright 2024 Nick Marino (github.com/nwmarino)

/*
 * The main driver script to the statim compiler, hosted in C++.
 *
 * This source code is distributed as is, without any warranty.
 */

#include <memory>
#include <string>

#include "include/lexer.h"
#include "include/parse.h"
#include "include/tstream.h"

int main(int argc, char *argv[])
{
  std::string path = "./samples/integer/ret/ret_0.statim";
  Lexer lex(path);

  std::shared_ptr<tstream> t_str = lex.tokenize();
  t_str->print();

  parse(t_str);
  
  return EXIT_SUCCESS;
}
