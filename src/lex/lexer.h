// Copyright (c) 2024 Nick Marino (github.com/nwmarino)

#ifndef LEXER_H
#define LEXER_H

#include <fstream>
#include <iostream>
#include <string>
#include <exception>
#include <vector>

#include "token.h"

using std::string;
using std::vector;

class Lexer {
  public:
    Lexer(string __srcpath);
    vector<Token> tokenize();

  private:
    string src;
};

#endif  // LEXER_H