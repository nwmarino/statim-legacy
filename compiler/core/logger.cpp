/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <memory>
#include <optional>
#include <string>

#include "logger.h"
#include <iostream>

void panic(const char *m, std::optional<const char *> arg) {
  fprintf(stderr, "statimc: %s\n", m);
  if (arg.has_value() && arg.value() != nullptr) {
    fprintf(stderr, "See: %s\n", arg.value_or("unknown"));
  }
  exit(1);
}


void sc_panic(const std::string m, const struct Metadata &data) {
  fprintf(stderr, "statimc: %s\n", m.c_str());
  fprintf(stderr, "See: %s:%zu\n", data.filename.c_str(), data.line_n);
  exit(1);
}


void tokexp_panic(TokenKind expected, const struct Metadata &data) {
  fprintf(stderr, "statimc: expected token %d\n", expected);
  fprintf(stderr, "See: %s:%zu\n", data.filename.c_str(), data.line_n);
  exit(1);
}


void symb_panic(std::string m, const struct Metadata &data) {
  fprintf(stderr, "statimc: %s\n", m.c_str());
  exit(1);
}


void symb_func_panic(const std::string &ident, const struct Metadata &data) {
  fprintf(stderr, "statimc: unresolved function identifier %s\n", ident.c_str());
  fprintf(stderr, "See: %s:%zu\n", data.filename.c_str(), data.line_n);
  exit(1);
}


void symb_var_panic(const std::string &ident, const struct Metadata &data) {
  fprintf(stderr, "statimc: unresolved variable identifier %s\n", ident.c_str());
  fprintf(stderr, "See: %s:%zu\n", data.filename.c_str(), data.line_n);
  exit(1);
}


void symb_decl_panic(const std::string &ident, const struct Metadata &data) {
  fprintf(stderr, "statimc: identifier already exists in this scope: %s\n", ident.c_str());
  fprintf(stderr, "See: %s:%zu\n", data.filename.c_str(), data.line_n);
  exit(1);
}


void symb_type_panic(const std::string &ident, const struct Metadata &data) {
  fprintf(stderr, "statimc: undeclared type: %s\n", ident.c_str());
  fprintf(stderr, "See: %s:%zu\n", data.filename.c_str(), data.line_n);
  exit(1);
}


std::unique_ptr<AST> warn(std::string m) {
  std::cout << "statimc: warn: " << m << '\n';
  return nullptr;
}


std::unique_ptr<Expr> warn_expr(std::string m) {
  warn(m);
  return nullptr;
}


std::unique_ptr<PrototypeAST> warn_proto(std::string m) {
  warn(m);
  return nullptr;
}


std::unique_ptr<Statement> warn_stmt(std::string m) {
  warn(m);
  return nullptr;
}


std::unique_ptr<Statement> warn_const(std::string m) {
  warn(m);
  return nullptr;
}
