/// Copyright 2024 Nick Marino (github.com/nwmarino)

#ifndef STATIMC_TOKEN_H
#define STATIMC_TOKEN_H

#include <memory>
#include <optional>
#include <utility>

/// An enum of common lexemmes.
typedef enum {
  /// Sequence of whitespace characters.
  Whitespace,

  /// Singular-line // comments.
  LineComment,

  /// Multi-line /* comments */.
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
  Period,
  /// ","
  Comma,
  /// ":",
  Colon,
  /// ";"
  Semicolon,
  /// "->"
  Arrow,
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
  /// "**"
  Power,
  /// "/"
  Slash,
  /// "="
  Eq,
  /// "==",
  EqEq,
  /// "!"
  Bang,
  /// "!="
  BangEq,
  /// "<"
  LessThan,
  /// "<="
  LessThanEq,
  /// ">"
  GreaterThan,
  /// ">="
  GreaterThanEq,
  /// "<<"
  LeftShift,
  /// ">>"
  RightShift,
  /// "&&"
  And,
  /// "||"
  Or,
  /// "^"
  Xor,
  /// "++"
  Increment,
  /// "--"
  Decrement,

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
  ByteString,
} LiteralKind;

/// Metadata about a token.
struct Metadata {
  const char *filename;
  std::size_t line_n;
};

/// A token representing a single lexeme.
struct Token {
  TokenKind kind;
  std::unique_ptr<Metadata> meta;
  std::optional<const char *> value;
  std::optional<LiteralKind> lit_kind;

  Token(TokenKind kind) : kind(kind) {};
  Token(
    TokenKind kind,
    std::unique_ptr<Metadata> meta,
    std::optional<const char *> value,
    std::optional<LiteralKind> lit_kind
  ) : kind(kind), meta(std::move(meta)), value(value), lit_kind(lit_kind) {};
};

#endif  // STATIMC_TOKEN_H
