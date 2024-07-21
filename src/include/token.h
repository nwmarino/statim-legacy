/// Copyright 2024 Nick Marino (github.com/nwmarino)

#ifndef STATIMC_TOKEN_H
#define STATIMC_TOKEN_H

#include <string>

/// An enumeration of all possible token types.
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
  IfKeyword,
  ElseKeyword,
  
  Boolean,
  Integer,
  Float,
  String,
  Char,

  Comment,
  Identifier,
  Terminate
} TokenType;

/// A token representing a single lexeme.
typedef
struct {
  std::string value;
  TokenType type;
} Token;

#endif  // STATIMC_TOKEN_H
