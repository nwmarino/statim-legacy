/// Copyright 2024 Nick Marino (github.com/nwmarino)

#ifndef STATIMC_LOGGER_H
#define STATIMC_LOGGER_H

#include <memory>
#include <optional>
#include <string>

#include "ast.h"
#include "token.h"

/**
 * Print an error message and exit the program.
 * 
 * @param m   The error message.
 * @param arg An optional argument to the error message.
 */
[[noreturn]]
void panic(const char *m, std::optional<const char *> arg);

/// Stop the compiler and print an error message with optional diagnostics.
/// @param m    The error message.
/// @param data Metadata about the bad input.
[[noreturn]]
void sc_panic(const std::string m, const struct Metadata &data);

/**
 * Print an error message regarding an expected token.
 * 
 * @param expected The expected token kind.
 * @param data     Metadata about the bad token.
 */
[[noreturn]]
void tokexp_panic(const std::string &expected, const struct Metadata &data);

/**
 * Print an error message regarding an unresolved symbol.
 * 
 * @param m    The error message.
 * @param data Metadata about the bad token.
 */
[[noreturn]]
void symb_panic(std::string m, const struct Metadata &data);

/**
 * Panic about an unresolved function identifier.
 * 
 * @param ident The function identifier.
 * @param data  Metadata about the bad token.
 */
[[noreturn]]
void symb_func_panic(const std::string &ident, const struct Metadata &data);

/**
 * Panic about an unresolved variable identifier.
 * 
 * @param ident The variable identifier.
 * @param data  Metadata about the bad token.
 */
[[noreturn]]
void symb_var_panic(const std::string &ident, const struct Metadata &data);

/**
 * Panic about a redefined symbol.
 * 
 * @param ident The identifier.
 * @param data  Metadata about the bad token.
 */
[[noreturn]]
void symb_decl_panic(const std::string &ident, const struct Metadata &data);


/**
 * Panic about an undeclared type.
 * 
 * @param ident The type name.
 * @param data  Metadata about the bad token.
 */
[[noreturn]]
void symb_type_panic(const std::string &ident, const struct Metadata &data);


/**
 * Panic about a reassigned constant.
 * 
 * @param ident The constant name.
 * @param data  Metadata about the bad token.
 */
[[noreturn]]
void symb_const_panic(const std::string &ident, const struct Metadata &data);

/**
 * Panic about a missing match expression.
 * 
 * @param data Metadata about the bad token.
 */
[[noreturn]]
void match_panic(const struct Metadata &data);

/**
 * Log an error message.
 * 
 * @param m The error message.
 * @return  nullptr
 */
std::unique_ptr<AST> warn(std::string m);

/**
 * Log an error message for an expression.
 * 
 * @param m The error message.
 * @return  nullptr
 */
std::unique_ptr<Expr> warn_expr(std::string m);

/**
 * Log an error message for a prototype.
 * 
 * @param m The error message.
 * @return  nullptr
 */
std::unique_ptr<PrototypeAST> warn_proto(std::string m);

/**
 * Log an error message for a statement.
 * 
 * @param m The error message.
 * @return  nullptr
 */
std::unique_ptr<Statement> warn_stmt(std::string m);

/**
 * Log an error message for an uninitialized constant.
 * 
 * @param m The error message.
 * @return  nullptr
 */
std::unique_ptr<Statement> warn_const(std::string m);

#endif  // STATIMC_LOGGER_H
