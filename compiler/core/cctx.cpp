/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <utility>

#include "cctx.h"
#include "token.h"  
#include "util.h"

cctx::cctx(cflags flags, std::vector<cfile> input) : flags(flags), input(input) {
  std::string f_src = read_to_str(input[0].path);
  std::string f_filename = input[0].filename;
  std::size_t f_len = f_src.size();

  this->lexer = std::make_unique<Tokenizer>(f_src, f_filename, f_len);
}

std::unique_ptr<Token> cctx::tk_next() {
  prev = std::make_unique<Token>(lexer->advance_token());
  return tk_prev();
}

inline std::unique_ptr<Token> cctx::tk_prev() {
  return std::move(prev);
}
