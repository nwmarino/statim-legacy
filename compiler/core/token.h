/// Copyright 2024 Nick Marino (github.com/nwmarino)

#ifndef STATIMC_TOKEN_H
#define STATIMC_TOKEN_H

#include <memory>
#include <optional>
#include <string>
#include <utility>

/// An enum of common lexemmes.
typedef enum {
  /// Finite sequence of whitespace characters.
  Whitespace,

  /// // line comments.
  LineComment,

  /// `/* block comments */`.
  BlockComment,

  /// A recognizable identifier.
  ///
  /// This includes keywords, variable names, and function names.
  Identifier,

  /// A constant literal.
  Literal,

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
  Dot,
  /// ","
  Comma,
  /// ":",
  Colon,
  /// ";"
  Semi,
  /// "+"
  Add,
  /// "-"
  Sub,
  /// "*"
  Star,
  /// "@"
  At,
  /// "#"
  Hash,
  /// "/"
  Slash,
  /// "="
  Eq,
  /// "!"
  Not,
  /// "<"
  LessThan,
  /// ">"
  GreaterThan,
  /// "&"
  And,
  /// "|"
  Or,
  /// "^"
  Xor,
  
  /// Compound tokens:
  ///
  /// "=="
  EqEq,
  /// "!="
  NotEq,
  /// "..."
  Range,
  /// "&&"
  AndAnd,
  /// "||"
  OrOr,
  /// "->"
  Arrow,
  /// "++"
  Increment,
  /// "--"
  Decrement,
  /// "+="
  AddEq,
  /// "-="
  SubEq,
  /// "*="
  StarEq,
  /// "/="
  SlashEq,
  /// "<="
  LessThanEq,
  /// ">="
  GreaterThanEq,
  /// "<<"
  LeftShift,
  /// ">>"
  RightShift,

  /// End of file.
  Eof
} TokenKind;

// An enum of recognized literal types.
typedef enum {
  /// true, false
  Bool,

  /// 1, 5, -1000
  Integer,

  /// 1.0, 1.0005, 0.0005
  Float,

  /// 'a', ';', '\\'
  Char,

  /// b'a', b';', b'\\'
  Byte,

  /// "hello", "world"
  String,

  /// b"hello", b"world"
  ByteString
} LiteralKind;

/// Metadata about a token.
struct Metadata {
  const std::string filename;
  std::size_t line_n;

  inline Metadata(const std::string &filename, std::size_t line_n)
    : filename(filename), line_n(line_n) {};
};

/// A token representing a single lexeme.
struct Token {
  TokenKind kind;
  std::unique_ptr<Metadata> meta;
  const std::string value;
  LiteralKind lit_kind;

  /// Constructor for basic tokens.
  inline Token(TokenKind kind) : kind(kind) {};

  /// Constructor for basic tokens with metadata.
  inline Token(TokenKind kind, std::unique_ptr<Metadata> meta) : kind(kind), meta(std::move(meta)) {};

  /// Constructor for tokens with necessary values.
  inline Token(TokenKind kind, std::unique_ptr<Metadata> meta, const std::string value)
    : kind(kind), meta(std::move(meta)), value(value) {};

  /// Constructor for literals.
  inline Token(TokenKind kind, std::unique_ptr<Metadata> meta, const std::string value, LiteralKind lit_kind)
    : kind(kind), meta(std::move(meta)), value(value), lit_kind(lit_kind) {};
};

inline bool is_ident(const Token &token) {
  return token.kind == Identifier;
};

inline bool is_lit(const Token &token) {
  return token.kind == Literal;
};

inline bool is_numer(const Token &token) {
  return token.lit_kind && (token.lit_kind == Integer || token.lit_kind == Float);
}

inline bool is_eof(const Token &token) {
  return token.kind == Eof;
};

inline bool is_whitespace(const Token &token) {
  return token.kind == Whitespace;
};

#endif  // STATIMC_TOKEN_H
