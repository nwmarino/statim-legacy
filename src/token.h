// Copyright (c) 2024 Nick Marino (github.com/nwmarino)

#ifndef TOKEN_H
#define TOKEN_H

#include <string>

using std::string;

typedef
enum {
  SetBrace,
  EndBrace,
  SetParen,
  EndParen,
  Separator,
  Semicolon,
  Comma,
  Arrow,

  AssignOperator,
  AddOperator,
  SubOperator,
  MultOperator,
  DivOperator,
  PowerOperator,

  BoolKeyword,
  IntKeyword,
  StringKeyword,
  FloatKeyword,
  CharKeyword,
  LetKeyword,
  FixKeyword,
  FunctionKeyword,
  ReturnKeyword,
  
  Boolean,
  Integer,
  Float,
  String,
  Char,

  Comment,
  Identifier,
  Terminate
} TokenType;

typedef
struct {
  string value;
  TokenType type;
} Token;

#endif  // TOKEN_H