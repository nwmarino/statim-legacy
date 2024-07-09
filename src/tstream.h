// Copyright (c) 2024 Nick Marino (github.com/nwmarino)

#ifndef TSTREAM_H
#define TSTREAM_H

#include <vector>

#include "token.h"

class tstream {
  std::vector<Token> __tokens;
  std::size_t __currit;

  public:
    tstream(std::vector<Token> __tokens);
    Token curr;
    void next();
    Token peek();
    std::size_t size();
    void print();
};

#endif  // TSTREAM_H