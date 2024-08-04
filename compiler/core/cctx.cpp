/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <utility>

#include "cctx.h"
#include "token.h"  
#include "util.h"

cctx::cctx(cflags flags, std::vector<cfile> input) : flags(flags), input(input) {
  std::string f_src = read_to_str(input[0].path);
  std::string f_filename = input[0].filename;
  std::size_t f_len = f_src.size();

  this->m_filename = f_filename;
  this->lexer = std::make_unique<Tokenizer>(f_src, f_filename, f_len);
}

std::unique_ptr<Token> cctx::tk_next() {
  m_prev = std::make_unique<Token>(lexer->advance_token());
  return prev();
}

void cctx::symb_add(const std::string &name, std::unique_ptr<Symbol> symbol) {
 this->symb_table->put(name, std::move(symbol));
}

std::unique_ptr<Symbol> cctx::symb_get(const std::string &name) {
  return std::move(symb_table->get(name));
}

bool cctx::symb_del(const std::string &name) {
  return symb_table->del(name);
}

bool cctx::symb_is(const std::string &name, SymbolType type) {
  return symb_get(name)->type == type;
}
