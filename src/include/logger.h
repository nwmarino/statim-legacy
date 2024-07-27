/// Copyright 2024 Nick Marino (github.com/nwmarino)

#ifndef STATIMC_LOGGER_H
#define STATIMC_LOGGER_H

#include <memory>
#include <string>

#include "ast.h"

/**
 * Log an error message.
 * 
 * @param m The error message.
 * @return  nullptr
 */
std::unique_ptr<Expr> logError(std::string m);

/**
 * Log an error message for a prototype.
 * 
 * @param m The error message.
 * @return  nullptr
 */
std::unique_ptr<PrototypeAST> logErrorPr(std::string m);

/**
 * Log an error message for a statement.
 * 
 * @param m The error message.
 * @return  nullptr
 */
std::unique_ptr<Statement> logErrorS(std::string m);

#endif  // STATIMC_LOGGER_H
