/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <cstdio>
#include <string>

#include "../core/logger.h"
#include "../core/token.h"
#include "tokenizer.h"

Tokenizer::Tokenizer(const std::string src, const std::string filename, std::size_t len)
  : src(src), filename(filename), len(len), prev('\0'), iter(0), line(1) {};

const struct Token Tokenizer::advance_token() {
  TokenKind kind = Eof;
  Metadata meta(filename, line);
  std::string value;
  LiteralKind lit_kind;

  if (iter >= len) {
    return Token(TokenKind::Eof, meta);
  }

  const char chr = src[iter];
  switch (chr)
  {
    /// Whitespace and new lines.
    case ' ':
    case '\t':
    case '\n':
      iter++;
      if (chr == '\n') {
        line++;
      }
      return advance_token();

    /// Slash or comment. Currently discard comments until doc support.
    case '/':
      if (peek() == '/') {
        while (src[iter] != '\n') {
          iter++;
        }
        return advance_token();
      } else if (peek() == '*') {
        while (iter < len && src[iter] != '*' && peek() != '/') {
          iter++;
        }
        return advance_token();
      }
      kind = Slash;

    /// Subtraction or thin arrow.
    case '-':
      kind = Sub;
      if (peek() == '>') {
        kind = Arrow;
        iter++;
      }
      break;
    
    /// Char literals.
    case '\'':
      kind = Literal;
      lit_kind = Char;
      iter++;

      value = src[iter];

      if (peek() != '\'') {
        sc_panic("Bad char literal", meta);
      }

      iter++;
      break;

    /// String literals.
    case '"':
      kind = Literal;
      lit_kind = String;

      while (peek() != '"') {
        value.push_back(src[iter]);
        iter++;
      }
      break;

    /// Dots or ranges.
    case '.':
      if (peek() == '.') {
        if (peek_two() != '.') {
          sc_panic("Bad range syntax", meta);
          break;
        }
        kind = Range;
        iter += 3;
      }
      kind = Dot;
      break;

    /// Equals, fat arrows, or comparison.
    case '=':
      if (peek() != '>' && peek() != '=') {
        kind = Eq;
        break;
      }

      kind = (peek() == '>') ? FatArrow : EqEq;
      iter++;
      break;

    /// One-character tokens.
    case '{': kind = OpenBrace; break;
    case '}': kind = CloseBrace; break;
    case '(': kind = OpenParen; break;
    case ')': kind = CloseParen; break;
    case '[': kind = OpenBracket; break;
    case ']': kind = CloseBracket; break;
    case ',': kind = Comma; break;
    case ';': kind = Semi; break;
    case ':': kind = Colon; break;
    case '+': kind = Add; break;
    case '*': kind = Star; break;
    case '@': kind = At; break;
    case '#': kind = Hash; break;
    case '!': kind = Not; break;
    case '<': kind = LessThan; break;
    case '>': kind = GreaterThan; break;
    case '&': kind = And; break;
    case '|': kind = Or; break;
    case '^': kind = Xor; break;

    /// Byte literals.
    case 'b':
      if (peek() == '\'') {
        kind = Literal;
        lit_kind = Byte;

        iter++;
        value = src[iter];

        if (peek() != '\'') {
          sc_panic("Bad byte char literal", meta);
        }

        iter++;
        break;

      } else if (peek() == '"') {
        kind = Literal;
        lit_kind = ByteString;

        while (peek() != '"') {
          value.push_back(src[iter]);
          iter++;
        }
        break;
      }

    /// Identifiers, numerics, unknowns.
    default:
      if (isalpha(chr)) {
        kind = Identifier;
        
        while (isalnum(src[iter]) || src[iter] == '_') {
          value.push_back(src[iter]);
          iter++;
        }

        if (value == "null") {
          kind = Literal;
          lit_kind = Null;
          return Token(kind, meta, value, lit_kind);
        }

        if (value == "true" || value == "false") {
          kind = Literal;
          lit_kind = Bool;
          return Token(kind, meta, value, lit_kind);
        }
        return Token(kind, meta, value);
      }
      else if (isdigit(chr)) {
        kind = Literal;
        lit_kind = Integer;

        while (isdigit(src[iter]) || src[iter] == '.') {
          if (src[iter] == '.' && lit_kind == Integer) {
            lit_kind = Float;
          }
          value.push_back(src[iter]);
          iter++;
        }
        return Token(kind, meta, value, lit_kind);
      }
      sc_panic("Unresolved sequence: " + std::string(1, chr), meta);
      break;
  }
  iter++;

  if (!lit_kind && value.empty()) {
    return Token(kind, meta);
  }

  if (!lit_kind) {
    return Token(kind, meta, value);
  }

  return Token(kind, meta, value, lit_kind);

}

const char Tokenizer::peek() const {
  return iter + 1 < len ? src[iter + 1] : '\0';
}

const char Tokenizer::peek_two() const {
  return iter + 2 < len ? src[iter + 2] : '\0';
}

const char Tokenizer::peek_three() const {
  return iter + 3 < len ? src[iter + 3] : '\0';
}

inline const std::string Tokenizer::to_str() {
  return src;
}

inline const bool Tokenizer::is_newl() {
  return prev == '\n';
}

inline const bool Tokenizer::is_eof() {
  return prev == '\0';
}

std::string Token::to_str() {
  switch (kind)
  {
    case LineComment: return "LineComment: " + value;
    case BlockComment: return "BlockComment: " + value;
    case Identifier: return "Identifier: " + value;
    case Literal: return "Literal: " + value;
    case Eof: return "Eof";
    case Slash: return "Slash";
    case Sub: return "Sub";
    case Arrow: return "Arrow";
    case Range: return "Range";
    case Dot: return "Dot";
    case Eq: return "Eq";
    case NotEq: return "NotEq";
    case FatArrow: return "FatArrow";
    case EqEq: return "EqEq";
    case OpenBrace: return "OpenBrace";
    case CloseBrace: return "CloseBrace";
    case OpenParen: return "OpenParen";
    case CloseParen: return "CloseParen";
    case OpenBracket: return "OpenBracket";
    case CloseBracket: return "CloseBracket";
    case Comma: return "Comma";
    case Semi: return "Semi";
    case Colon: return "Colon";
    case Add: return "Add";
    case Star: return "Star";
    case At: return "At";
    case Hash: return "Hash";
    case Not: return "Not";
    case LessThan: return "LessThan";
    case GreaterThan: return "GreaterThan";
    case And: return "And";
    case AndAnd: return "AndAnd";
    case Or: return "Or";
    case OrOr: return "OrOr";
    case Xor: return "Xor";
    case Increment: return "Increment";
    case Decrement: return "Decrement";
    case AddEq: return "AddEq";
    case SubEq: return "SubEq";
    case StarEq: return "StarEq";
    case SlashEq: return "SlashEq";
    case LessThanEq: return "LessThanEq";
    case GreaterThanEq: return "GreaterThanEq";
    case LeftShift: return "LeftShift";
    case RightShift: return "RightShift";
    default: return "Unknown";
  }
}
