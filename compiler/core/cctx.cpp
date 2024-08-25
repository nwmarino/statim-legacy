/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include "cctx.h"
#include "token.h"  
#include "util.h"
#include <iostream>

cctx::cctx(cflags flags, std::vector<cfile> input) : flags(flags), input(input), m_prev(Token(Eof)) {
  // add primitive types to the symbol table
  symb_add("bool", Symbol(SymbolType::Ty));
  symb_add("i32", Symbol(SymbolType::Ty));
  symb_add("u32", Symbol(SymbolType::Ty));
  symb_add("i64", Symbol(SymbolType::Ty));
  symb_add("u64", Symbol(SymbolType::Ty));
  symb_add("float", Symbol(SymbolType::Ty));
  symb_add("float64", Symbol(SymbolType::Ty));
  symb_add("char", Symbol(SymbolType::Ty));
  symb_add("byte", Symbol(SymbolType::Ty));
  symb_add("str", Symbol(SymbolType::Ty));
  symb_add("bstr", Symbol(SymbolType::Ty));

  // add keywords to the symbol table
  symb_add("abstract", Symbol(KeywordType::Abstract));
  symb_add("bind", Symbol(KeywordType::Bind));
  symb_add("break", Symbol(KeywordType::Break));
  symb_add("continue", Symbol(KeywordType::Continue));
  symb_add("else", Symbol(KeywordType::Else));
  symb_add("end", Symbol(KeywordType::End));
  symb_add("enum", Symbol(KeywordType::Enum));
  symb_add("extern", Symbol(KeywordType::Extern));
  symb_add("false", Symbol(KeywordType::False));
  symb_add("fix", Symbol(KeywordType::Fix));
  symb_add("fn", Symbol(KeywordType::Fn));
  symb_add("for", Symbol(KeywordType::For));
  symb_add("if", Symbol(KeywordType::If));
  symb_add("impl", Symbol(KeywordType::Impl));
  symb_add("let", Symbol(KeywordType::Let));
  symb_add("match", Symbol(KeywordType::Match));
  symb_add("pkg", Symbol(KeywordType::Pkg));
  symb_add("protect", Symbol(KeywordType::Protect));
  symb_add("return", Symbol(KeywordType::Return));
  symb_add("struct", Symbol(KeywordType::Struct));
  symb_add("true", Symbol(KeywordType::True));
  symb_add("union", Symbol(KeywordType::Union));
  symb_add("until", Symbol(KeywordType::Until));
  symb_add("use", Symbol(KeywordType::Use));
}


struct Token cctx::tk_next() {
  m_prev = lexer->advance_token();
  return m_prev;
}


void cctx::tk_expect(TokenKind kind, const std::string &msg) {
  if (this->m_prev.kind != kind) {
    tokexp_panic(msg, this->m_prev.meta);
  }
  this->tk_next(); // eat the token
}


void cctx::assert_ident() {
  if (this->m_prev.kind != TokenKind::Identifier) {
    tokexp_panic("identifier", this->m_prev.meta);
  }
}


void cctx::file_next() {
  if (input.size() != 0) {
    std::string f_src = read_to_str(input.at(input.size() - 1).path);
    std::string f_filename = input.at(input.size() - 1).filename;
    std::size_t f_len = f_src.size();

    this->lexer = std::make_unique<Tokenizer>(f_src, f_filename, f_len);
    this->m_filename = f_filename;
    this->tk_next();

    input.pop_back();
  }
}


void cctx::symb_add(const std::string &name, const struct Symbol &symbol) {
  if (symb_table->exists(name)) {
    sc_panic("identifier already exists: " + name, this->prev().meta);
  }

  symb_table->put(name, symbol);
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


bool cctx::symb_is_kw(const std::string &name, KeywordType kw) {
  return symb_is(name, SymbolType::Keyword) && symb_get(name).keyword == kw;
}
