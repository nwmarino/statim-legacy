// Copyright (c) 2024 Nick Marino (github.com/nwmarino)

#include <string>
#include <iostream>
#include <memory>
#include <utility>

#include "lexer.h"
#include "tstream.h"
//#include "parser.h"

int main() {
  std::string path("../test/function/main.statim");
  Lexer lex(path);

  std::shared_ptr<tstream> tbuf = lex.tokenize();

  // (*cc).print();

  //parse(tokens);
}
