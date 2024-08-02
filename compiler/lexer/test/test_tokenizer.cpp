/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include "../tokenizer.h"

/// Test tokenizing a simple prototype.
[[nodiscard]]
bool test_prototype() {
  std::string src = "fn main() -> int {}";
  Tokenizer tokenizer(src, "main.statim", src.size());

  const std::string expected = "2,2,6,7,32,2,4,5";
  std::string actual;

  while (1) {
    Token token = tokenizer.advance_token();
    if (token.kind == TokenKind::Eof) {
      actual.resize(actual.size() - 1);
      break;
    }
    actual += std::to_string(token.kind) + ",";
  }
  return expected == actual;
}
