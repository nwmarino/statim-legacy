#ifndef STATIMC_TOKEN_H
#define STATIMC_TOKEN_H

/// Recognized token and token information.
/// Copyright 2024 Nick Marino (github.com/nwmarino)

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

  /// "hello", "world"
  String,
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

  /// Determine if this token is an identifier or not.
  [[nodiscard]]
  inline bool is_ident() const { return kind == Identifier; };

  /// Determine if this token is a literal or not.
  [[nodiscard]]
  inline bool is_lit() const { return kind == Literal; };

  /// Determine if this token is an integer literal or not.
  [[nodiscard]]
  inline bool is_int() const { return is_lit() && lit_kind && lit_kind == Integer; };

  /// Determine if this token is a boolean literal or not.
  [[nodiscard]]
  inline bool is_bool() const { return is_lit() && lit_kind && lit_kind == Bool; };

  /// Determine if this token is a floating point literal or not.
  [[nodiscard]]
  inline bool is_float() const { return is_lit() && lit_kind && lit_kind == Float; };

  /// Determine if this token is a character literal or not.
  [[nodiscard]]
  inline bool is_char() const { return is_lit() && lit_kind && lit_kind == Char; };

  /// Determine if this token is a string literal or not.
  [[nodiscard]]
  inline bool is_str() const { return is_lit() && lit_kind && lit_kind == String; };

  /// Determine if this token signifies the end of a file.
  [[nodiscard]]
  inline bool is_eof() const { return kind == Eof; };

  /// Determine if this token is keyword or not.
  [[nodiscard]]
  inline bool is_kw(const std::string &value) const { return is_ident() && this->value == value; };

  /// Determine if this token is an opening brace or not.
  [[nodiscard]]
  inline bool is_open_brace() const { return kind == OpenBrace; };

  /// Determine if this token is a closing brace or not.
  [[nodiscard]]
  inline bool is_close_brace() const { return kind == CloseBrace; };

  /// Determine if this token is an opening parenthesis or not.
  [[nodiscard]]
  inline bool is_open_paren() const { return kind == OpenParen; };

  /// Determine if this token is a closing parenthesis or not.
  [[nodiscard]]
  inline bool is_close_paren() const { return kind == CloseParen; };

  /// Determine if this token is a semi or not.
  [[nodiscard]]
  inline bool is_semi() const { return kind == Semi; };

  /// Determine if this token is a colon or not.
  [[nodiscard]]
  inline bool is_colon() const { return kind == Colon; };

  /// Determine if this token is a comma or not.
  [[nodiscard]]
  inline bool is_comma() const { return kind == Comma; };

  /// Determine if this token is an equal sign or not.
  [[nodiscard]]
  inline bool is_eq() const { return kind == Eq; };

  /// Determine if this token is an arrow or not.
  [[nodiscard]]
  inline bool is_arrow() const { return kind == Arrow; };

  /// Determine if this token is a fat arrow or not.
  [[nodiscard]]
  inline bool is_fat_arrow() const { return kind == FatArrow; };

  /// Determine if this token is a hash or not.
  [[nodiscard]]
  inline bool is_hash() const { return kind == Hash; };

  /// Determine if this token is a dot or not.
  [[nodiscard]]
  inline bool is_dot() const { return kind == Dot; };

  /// Returns a string representation of this token.
  [[nodiscard]]
  std::string to_str();
};

#endif  // STATIMC_TOKEN_H
