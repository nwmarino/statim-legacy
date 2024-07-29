// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <iostream>
#include <memory>
#include <string>

#include "../include/ast.h"
#include "../include/logger.h"

/**
 * Print an error message and exit the program.
 * 
 * @param m   The error message.
 * @param arg An optional argument to the error message.
 */
void panic(const char *m, std::optional<const char *> arg)
{
  fprintf(stderr, "%s", m);
  fprintf(stderr, "See: %s", arg.value_or("unknown"));
  exit(1);
}

/**
 * Log an error message.
 * 
 * @param m The error message.
 * @return  nullptr
 */
std::unique_ptr<Expr> logError(std::string m) {
  std::cout << "Error: " << m << '\n';
  return nullptr;
}

/**
 * Log an error message for a prototype.
 * 
 * @param m The error message.
 * @return  nullptr
 */
std::unique_ptr<PrototypeAST> logErrorPr(std::string m) {
  logError(m);
  return nullptr;
}

/**
 * Log an error message for a statement.
 * 
 * @param m The error message.
 * @return  nullptr
 */
std::unique_ptr<Statement> logErrorS(std::string m) {
  logError(m);
  return nullptr;
}
