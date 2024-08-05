/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <iostream>
#include <vector>

#include "../../core/token.h"
#include "../tokenizer.h"

[[nodiscard]]
bool test_return_zero() {
  std::string src = "fn main() -> int { return 0; }";
  Tokenizer tokenizer(src, "main.statim", src.size());

  std::vector<TokenKind> expected = {
    Identifier,
    Identifier,
    OpenParen,
    CloseParen,
    Arrow,
    Identifier,
    OpenBrace,
    Identifier,
    Literal,
    Semi,
    CloseBrace
  };
  std::vector<TokenKind> actual;

  while (1) {
    Token token = tokenizer.advance_token();
    if (token.kind == TokenKind::Eof) {
      actual.resize(actual.size() - 1);
      break;
    }
    actual.push_back(token.kind);
  }
  return expected == actual;
}