/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include "llvm/IR/Value.h"

#include <iostream>
#include <memory>
#include <string>

#include "../include/logger.h"
#include "../include/ast.h"

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


/**
 * Log an error message for a value.
 * 
 * @param m The error message.
 * @return  nullptr
 */
llvm::Value *logErrorV(std::string m) {
  logError(m);
  return nullptr;
}
