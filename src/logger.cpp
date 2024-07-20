// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <string>
#include <memory>
#include <iostream>

#include "logger.h"
#include "ast.h"
#include "llvm/IR/Value.h"

std::unique_ptr<Expr> logError(std::string m)
{
  std::cout << "Error: " << m << '\n';
  return nullptr;
}

std::unique_ptr<PrototypeAST> logErrorPr(std::string m)
{
  logError(m);
  return nullptr;
}

std::unique_ptr<Statement> logErrorS(std::string m)
{
  logError(m);
  return nullptr;
}

llvm::Value *logErrorV(std::string m)
{
  logError(m);
  return nullptr;
}
