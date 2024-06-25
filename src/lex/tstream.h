// Copyright (c) 2024 Nick Marino (github.com/nwmarino)

#ifndef TSTREAM_H
#define TSTREAM_H

#include <vector>

#include "token.h"

using std::vector;

class tstream {
  public:
    tstream(vector<Token> __tokens);

  private:
    vector<Token> __tokens;
};

#endif  // TSTREAM_H