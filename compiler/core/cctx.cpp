/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include "cctx.h"
#include "token.h"  
#include "util.h"

cctx::cctx(cflags flags, std::vector<cfile> input) : flags(flags), input(input), m_prev(Token(Eof)) {
  std::string f_src = read_to_str(input[0].path);
  std::string f_filename = input[0].filename;
  std::size_t f_len = f_src.size();
  
  this->m_filename = f_filename;
  this->lexer = std::make_unique<Tokenizer>(f_src, f_filename, f_len);

  // add primitive types to the symbol table
  symb_add("bool", Symbol(SymbolType::Ty));
  symb_add("i32", Symbol(SymbolType::Ty));
  symb_add("i64", Symbol(SymbolType::Ty));

  // add keywords to the symbol table
  symb_add("fn", Symbol(KeywordType::Fn));
  symb_add("let", Symbol(KeywordType::Let));
  symb_add("fix", Symbol(KeywordType::Fix));
  symb_add("if", Symbol(KeywordType::If));
  symb_add("else", Symbol(KeywordType::Else));
  symb_add("return", Symbol(KeywordType::Return));
}

struct Token cctx::tk_next() {
  m_prev = lexer->advance_token();
  return m_prev;
}

void cctx::symb_add(const std::string &name, const struct Symbol &symbol) {
 this->symb_table->put(name, symbol);
}

struct Symbol cctx::symb_get(const std::string &name) {
  return symb_table->get(name);
}

bool cctx::symb_exists(const std::string &name) {
  return symb_table->exists(name);
}

bool cctx::symb_del(const std::string &name) {
  return symb_table->del(name);
}

bool cctx::symb_is(const std::string &name, SymbolType type) {
  return symb_exists(name) && symb_get(name).type == type;
}
