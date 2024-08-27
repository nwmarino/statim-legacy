/// Copyright 2024 Nick Marino (github.com/nwmarino)

#ifndef STATIMC_TOKEN_H
#define STATIMC_TOKEN_H

#include <string>

/// An enum of common lexemmes.
typedef enum {
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
  /// "^^"
  XorXor,
  /// "->"
  Arrow,
  /// "=>"
  FatArrow,
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
  /// Constant Literals:

  /// null
  Null,

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
  std::string filename;
  std::size_t line_n;
  std::size_t col_n;

  /// Default constructor.
  inline Metadata() : filename("unknown"), line_n(0), col_n(0) {};

  /// Constructor for basic token metadata.
  inline Metadata(const std::string &filename, const std::size_t line_n, const std::size_t col_n)
    : filename(filename), line_n(line_n), col_n(col_n) {};

  /// Copy constructor.
  inline Metadata(const Metadata &meta)
    : filename(meta.filename), line_n(meta.line_n), col_n(meta.col_n) {};

  /// Assignment operator.
  inline Metadata &operator=(const Metadata &meta) {
    filename = meta.filename;
    line_n = meta.line_n;
    col_n = meta.col_n;
    return *this;
  }
};

/// A token representing a single lexeme.
struct Token {
  TokenKind kind;
  struct Metadata meta;
  std::string value;
  LiteralKind lit_kind;
  
  /// Constructor for basic tokens.
  inline Token(TokenKind kind) : kind(kind) {};

  /// Constructor for basic tokens with metadata.
  inline Token(TokenKind kind, Metadata &meta) : kind(kind), meta(meta) {};

  /// Constructor for tokens with necessary values.
  inline Token(TokenKind kind, Metadata &meta, const std::string value)
    : kind(kind), meta(meta), value(value) {};

  /// Constructor for literals.
  inline Token(TokenKind kind, Metadata &meta, const std::string value, LiteralKind lit_kind)
    : kind(kind), meta(meta), value(value), lit_kind(lit_kind) {};

  /// Copy constructor.
  inline Token(const Token &token)
    : kind(token.kind), meta(token.meta), value(token.value), lit_kind(token.lit_kind) {};

  /// Assignment operator.
  inline Token &operator=(const Token &token) {
    kind = token.kind;
    meta = token.meta;
    value = token.value;
    lit_kind = token.lit_kind;
    return *this;
  }

  [[nodiscard]]
  inline const bool is_ident() { return kind == Identifier; };

  [[nodiscard]]
  inline const bool is_lit() { return kind == Literal; };

  [[nodiscard]]
  inline const bool is_numer() { return lit_kind && (lit_kind == Integer || lit_kind == Float); }

  [[nodiscard]]
  inline const bool is_eof() { return kind == Eof; };

  [[nodiscard]]
  std::string to_str();
};

#endif  // STATIMC_TOKEN_H
