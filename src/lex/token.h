// Copyright (c) 2024 Nick Marino (github.com/nwmarino)

#ifndef TOKEN_H
#define TOKEN_H

#include <string>

using std::string;

typedef
enum {
  START_BLOCK,
  END_BLOCK,
  START_PARAM,
  END_PARAM,
  SEMICOLON,
  ARROW,
  KEYWORD,
  IDENTIFIER,
  BOOLEAN,
  INTEGER,
  END_STREAM
} TokenType;

typedef
struct {
  string value;
  TokenType type;
} Token;

#endif  // TOKEN_H