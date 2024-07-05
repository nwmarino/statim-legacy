// Copyright (c) 2024 Nick Marino (github.com/nwmarino)

#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <string>
#include <memory>
#include "ast.h"

std::unique_ptr<Expr> logError(std::string m);
std::unique_ptr<FunctionPrototype> logErrorPr(std::string m);

#endif  // LOGGER_H