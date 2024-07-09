// Copyright (c) 2024 Nick Marino (github.com/nwmarino)

#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <memory>

#include "token.h"
#include "tstream.h"

class Lexer {
  std::string src;
  int iter;
  Token tokenize_id();
  Token tokenize_numerical();
  Token tokenize_string();
  Token tokenize_char();

  public:
    Lexer(string __srcpath);
    std::shared_ptr<tstream> tokenize();
};

#endif  // LEXER_H
