/// Copyright 2024 Nick Marino (github.com/nwmarino)

#ifndef STATIMC_TOKEN_H
#define STATIMC_TOKEN_H

#include <string>

typedef enum {
  A,
  B
} LiteralType;

/// An enumeration of all possible token types.
typedef enum {
  /// Sequence of whitespace characters.
  Whitespace,

  /// Singular-line comments.
  LineComment,

  /// Multi-line comments.
  BlockComment,

  /// A recognizable identifier.
  Identifier,

  /// A constant literal.
  Literal { LiteralType type },

  SET_BLOCK,
  SET_PAREN,
  END_BLOCK,
  END_PAREN,
  SEPARATOR,
  SEMICOLON,
  COMMA,
  ARROW,
  OP_ASSIGN,
  OP_POW,
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

  
 
  /// One-character tokens:
  ///
  /// "{"
  OpenBrace,
  /// "}"
  CloseBrace,
  /// "("
  OpenParen,
  /// ")"
  CloseParen,
  /// "["
  OpenBracket,
  /// "]"
  CloseBracket,
  /// "."
  Period,
  /// ","
  Comma,
  /// ":",
  Colon,
  /// ";"
  Semicolon,
  /// "+"
  Add,
  /// "-"
  Sub,
  /// "*"
  Star,
  /// "/"
  Slash,
  /// "="
  Assign,
  /// "==",
  Equals,
  /// "!"
  Bang,
  /// "!="
  BangEquals,
  /// "<"
  LessThan,
  /// "<="
  LessThanEquals,
  /// ">"
  GreaterThan,
  /// ">="
  GreaterThanEquals,
  /// "&&"
  And,
  /// "||"
  Or,
  /// "++"
  Increment,
  /// "--"
  Decrement,
  /// "^"
  Xor,

  /// End of input.
  Eof
} TokenType;

/// A token representing a single lexeme.
typedef struct {
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
