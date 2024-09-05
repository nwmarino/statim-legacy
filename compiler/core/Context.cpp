#include "../include/core/ASTContext.h"
#include "../include/core/Utils.h"

ASTContext::ASTContext(struct CFlags flags, std::vector<struct CFile> input)
: flags(flags), input(input), _last(Token(Eof)), _last_two(Token(Eof)), type_table({}) {
  // load built-in types
}


struct Token ASTContext::next(void) {
  _last_two = _last;
  return _last = lexer->advance_token(); 
}


void ASTContext::next_file(void) {
  if (input.size() != 0) {
      std::string f_src = read_to_str(input.at(input.size() - 1).path);
      _file = input.at(input.size() - 1).filename;
      lexer = std::make_unique<Tokenizer>(f_src, input.at(input.size() - 1).filename, f_src.size());
      next();
      input.pop_back();
  }
}


inline struct Token ASTContext::last(void) const { return _last; }
inline struct Token ASTContext::last_two(void) const { return _last_two; }
inline std::string ASTContext::file(void) const { return _file; }
