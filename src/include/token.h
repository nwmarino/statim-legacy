/// Copyright 2024 Nick Marino (github.com/nwmarino)

#ifndef STATIMC_TOKEN_H
#define STATIMC_TOKEN_H

#include <string>

/// An enumeration of all possible token types.
typedef
enum {
  SET_BLOCK,
  SET_PAREN,
  END_BLOCK,
  END_PAREN,
  SEPARATOR,
  SEMICOLON,
  COMMA,
  ARROW,

  OP_ASSIGN,
  OP_ADD,
  OP_SUB,
  OP_MULT,
  OP_DIV,
  OP_POW,

  C_BOOL,
  C_INT,
  C_FP,
  C_STR,
  C_CHAR,

  BOOL_KEYWORD,
  CHAR_KEYWORD,
  ELSE_KEYWORD,
  FIX_KEYWORD,
  FLOAT_KEYWORD,
  FUNCTION_KEYWORD,
  IF_KEYWORD,
  INT_KEYWORD,
  LET_KEYWORD,
  STRING_KEYWORD,
  RETURN_KEYWORD,

  COMMENT,
  IDENTIFIER,
  TERMINATE_STREAM
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
  return tok.type == BOOL_KEYWORD || tok.type == INT_KEYWORD
      || tok.type == STRING_KEYWORD || tok.type == FLOAT_KEYWORD
      || tok.type == CHAR_KEYWORD || tok.type == LET_KEYWORD
      || tok.type == FIX_KEYWORD || tok.type == FUNCTION_KEYWORD
      || tok.type == RETURN_KEYWORD || tok.type == IF_KEYWORD
      || tok.type == ELSE_KEYWORD;
}

#endif  // STATIMC_TOKEN_H
