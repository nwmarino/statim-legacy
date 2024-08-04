/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <iostream>
#include <memory>
#include <optional>
#include <string>

#include "ast.h"
#include "logger.h"

void sc_panic(const std::string m, std::unique_ptr<Metadata> data) {
  fprintf(stderr, "statimc: %s\n", m.c_str());
  if (data) {
    fprintf(stderr, "See: %s:%zu\n", data->filename.c_str(), data->line_n);
  }
  exit(1);
}

void panic(const char *m, std::optional<const char *> arg) {
  fprintf(stderr, "statimc: %s\n", m);
  if (arg.has_value() && arg.value() != nullptr) {
    fprintf(stderr, "See: %s\n", arg.value_or("unknown"));
  }
  exit(1);
}

void tokexp_panic(TokenKind expected, std::unique_ptr<Metadata> data) {
  fprintf(stderr, "statimc: expected token %d\n", expected);
  if (data) {
    fprintf(stderr, "See: %s:%zu\n", data->filename.c_str(), data->line_n);
  }
  exit(1);
}

void symb_panic(std::string m, std::unique_ptr<Metadata> data) {
  fprintf(stderr, "statimc: %s\n", m.c_str());
  exit(1);
}

void symb_func_panic(const std::string &ident, std::unique_ptr<Metadata> data) {
  fprintf(stderr, "statimc: unresolved function identifier %s\n", ident.c_str());
  if (data) {
    fprintf(stderr, "See: %s:%zu\n", data->filename.c_str(), data->line_n);
  }
  exit(1);
}

void symb_var_panic(const std::string &ident, std::unique_ptr<Metadata> data) {
  fprintf(stderr, "statimc: unresolved variable identifier %s\n", ident.c_str());
  if (data) {
    fprintf(stderr, "See: %s:%zu\n", data->filename.c_str(), data->line_n);
  }
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
