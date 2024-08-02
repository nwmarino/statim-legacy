/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <iostream>
#include <memory>
#include <optional>
#include <string>

#include "ast.h"
#include "logger.h"

void sc_panic(const char *m, std::optional<Metadata> data) {
  fprintf(stderr, "%s", m);
  fprintf(stderr, "See: %s:%zu", data->filename.c_str(), data->line_n);
  exit(1);
}

void panic(const char *m, std::optional<const char *> arg) {
  fprintf(stderr, "%s", m);
  fprintf(stderr, "See: %s", arg.value_or("unknown"));
  exit(1);
}

std::unique_ptr<Expr> logError(std::string m) {
  std::cout << "Error: " << m << '\n';
  return nullptr;
}

std::unique_ptr<PrototypeAST> logErrorPr(std::string m) {
  logError(m);
  return nullptr;
}

std::unique_ptr<Statement> logErrorS(std::string m) {
  logError(m);
  return nullptr;
}
