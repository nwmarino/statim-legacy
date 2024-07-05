// Copyright (c) 2024 Nick Marino (github.com/nwmarino)

#ifndef TSTREAM_H
#define TSTREAM_H

#include <vector>

#include "token.h"

using std::vector;
using std::size_t;

class tstream {
  vector<Token> __tokens;
  int __currit;

  public:
    tstream(vector<Token> __tokens);
    Token curr;
    void next();
    size_t size();
    string debugstr();
};

#endif  // TSTREAM_H