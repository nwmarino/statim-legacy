#include "../include/core/ASTContext.h"
#include "../include/core/Type.h"
#include "../include/core/Utils.h"

ASTContext::ASTContext(struct CFlags flags, std::vector<struct CFile> input)
: flags(flags), input(input), _last(Token(Eof)), _last_two(Token(Eof)), type_table({}) {
  // load built-in types
  type_table["bool"] = new PrimitiveType(PrimitiveType::PrimitiveKind::__UINT1);
  type_table["uint"] = new PrimitiveType(PrimitiveType::PrimitiveKind::__UINT32);
  type_table["i32"] = new PrimitiveType(PrimitiveType::PrimitiveKind::__INT32);
  type_table["i64"] = new PrimitiveType(PrimitiveType::PrimitiveKind::__INT64);
  type_table["float"] = new PrimitiveType(PrimitiveType::PrimitiveKind::__FP32);
  type_table["char"] = new PrimitiveType(PrimitiveType::PrimitiveKind::__CHAR);
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


Type* ASTContext::resolve_type(const std::string &name) {
  if (type_table.find(name) != type_table.end()) {
    return type_table.at(name);
  }
  type_table[name] = new TypeRef(name);
  return type_table.at(name);
}


inline struct Token ASTContext::last(void) const { return _last; }
inline struct Token ASTContext::last_two(void) const { return _last_two; }
inline std::string ASTContext::file(void) const { return _file; }
