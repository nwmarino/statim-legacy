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

  Boolean,
  Integer,
  Float,
  String,
  Char,

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


/**
 * Check if a token is a keyword.
 * 
 * @param tok Token to check.
 * @return    True if the token is a keyword, false otherwise.
 */
inline bool isKeyword(Token tok) {
  return tok.type == BoolKeyword || tok.type == IntKeyword
      || tok.type == StringKeyword || tok.type == FloatKeyword
      || tok.type == CharKeyword || tok.type == LetKeyword
      || tok.type == FixKeyword || tok.type == FunctionKeyword
      || tok.type == ReturnKeyword || tok.type == IfKeyword
      || tok.type == ElseKeyword;
}

#endif  // STATIMC_TOKEN_H
