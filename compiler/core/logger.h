/// Copyright 2024 Nick Marino (github.com/nwmarino)

#ifndef STATIMC_LOGGER_H
#define STATIMC_LOGGER_H

#include <memory>
#include <optional>
#include <string>

#include "ast.h"
#include "token.h"

/// Stop the compiler and print an error message with optional diagnostics.
/// @param m    The error message.
/// @param data Metadata about the bad input.
[[noreturn]]
void sc_panic(const char *m, std::optional<Metadata> data);

/**
 * Print an error message and exit the program.
 * 
 * @param m   The error message.
 * @param arg An optional argument to the error message.
 */
[[noreturn]]
void panic(const char *m, std::optional<const char *> arg);

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
