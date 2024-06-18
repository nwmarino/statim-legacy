// Copyright (c) 2024 Nick Marino (github.com/nwmarino)

#ifndef LEXER_H
#define LEXER_H

#include <iostream>
#include <fstream>
#include <string>

using std::fstream;
using std::string;

class Lexer {
  public:
    Lexer(string __srcpath);

  private:
    string src;
};

#endif  // LEXER_H