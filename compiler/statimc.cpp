/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <iostream>

#include "lexer/tokenizer.h"
#include "core/token.h"

int main(int argc, char *argv[]) {
  std::string src = "fn main() -> int { return 0; }";
  Tokenizer tokenizer(src, "main.statim", src.size());

  while (1) {
    Token token = tokenizer.advance_token();
    if (token.kind == TokenKind::Eof) {
      break;
    }
    std::cout << token.to_str() << '\n';
  }
  return 0;
}
