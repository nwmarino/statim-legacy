// Copyright (c) 2024 Nick Marino (github.com/nwmarino)

#ifndef LEXER_H
#define LEXER_H

#include <string>

#include "token.h"
#include "tstream.h"

using std::string;

class Lexer {
  public:
    Lexer(string __srcpath);
    tstream tokenize();
    Token tokenize_id();
    Token tokenize_numerical();

  private:
    string src;
    int iter;
};

#endif  // LEXER_H