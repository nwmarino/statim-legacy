// Copyright (c) 2024 Nick Marino (github.com/nwmarino)

#include <vector>

#include "token.h"
#include "tstream.h"

using std::vector;
using std::size_t;

tstream::tstream(vector<Token> __tokens) {
  Token terminate;
  terminate.type = END_STREAM;
  __tokens.push_back(terminate);
  
  this->__tokens = __tokens;
  this->__currit = 0;
}

Token tstream::next() {
  if (__tokens[__currit].type == END_STREAM) {
    return __tokens[__currit];
  }
  __currit++;
  return __tokens[__currit - 1];
}

size_t tstream::size() {
  return __tokens.size();
}
