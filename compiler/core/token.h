/// Copyright 2024 Nick Marino (github.com/nwmarino)

#ifndef STATIMC_TOKEN_H
#define STATIMC_TOKEN_H

#include <memory>
#include <string>
#include <utility>

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
  const std::string filename;
  const std::size_t line_n;

  /// Constructor for basic token metadata.
  inline Metadata(const std::string &filename, const std::size_t line_n)
    : filename(filename), line_n(line_n) {};

  /// Copy constructor.
  inline Metadata(const Metadata &meta)
    : filename(meta.filename), line_n(meta.line_n) {};
};

/// A token representing a single lexeme.
struct Token {
  TokenKind kind;
  std::unique_ptr<Metadata> meta;
  std::string value;
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

  /// Copy constructor.
  inline Token(const Token &token)
    : kind(token.kind), meta(std::make_unique<Metadata>(*token.meta)), value(token.value), lit_kind(token.lit_kind) {};

  /// Assignment operator.
  inline Token &operator=(const Token &token) {
    kind = token.kind;
    meta = std::make_unique<Metadata>(*token.meta);
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

// An enum of recognized keywords.
typedef enum KeywordType {
  /// fn
  Fn,

  /// let
  Let,

  /// fix
  Fix,

  /// if
  If,

  /// else
  Else,

  /// while
  While,

  /// for
  For,

  /// return
  Return,

  /// break
  Break,

  /// continue
  Continue,

  /// match
  Match
} KeywordType;

#endif  // STATIMC_TOKEN_H
