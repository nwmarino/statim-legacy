// Copyright (c) 2024 Nick Marino (github.com/nwmarino)

#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <memory>

#include "ast.h"
#include "llvm/IR/Value.h"

std::unique_ptr<Expr> logError(std::string m);
std::unique_ptr<PrototypeAST> logErrorPr(std::string m);
std::unique_ptr<Statement> logErrorS(std::string m);
llvm::Value *logErrorV(std::string m);

#endif  // LOGGER_H