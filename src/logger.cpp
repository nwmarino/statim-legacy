// Copyright (c) 2024 Nick Marino (github.com/nwmarino)

#include <memory>

#include "logger.h"
#include "ast.h"

std::unique_ptr<Expr> logError(std::string m) {
  std::cout << "ERR: " << m << '\n';
  return nullptr;
}

std::unique_ptr<FunctionPrototype> logErrorPr(std::string m) {
  logError(m);
  return nullptr;
}
