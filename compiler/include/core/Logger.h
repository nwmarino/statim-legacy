#ifndef STATIMC_LOGGER_H
#define STATIMC_LOGGER_H

/// Suite of logging functions for the compiler.
/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include "../ast/Decl.h"
#include "../ast/Stmt.h"
#include "../ast/Unit.h"
#include "../token/Token.h"

/// Stop the compiler and print an error message.
/// @param m    The error message.
[[noreturn]]
inline void panic(const std::string msg) {
  fprintf(stderr, "statimc: panic: %s\n", msg.c_str());
  exit(1);
}


/// Stop the compiler and print an error message with location information.
/// @param m    The error message.
/// @param data Metadata about the bad input.
[[noreturn]]
inline void panic(const std::string msg, const struct Metadata &data) {
  fprintf(stderr, "statimc: panic: %s\nsee: %s:%zu:%zu\n", msg.c_str(), data.filename.c_str(), data.line_n, data.col_n);
  exit(1);
}


/// Panic about an unexpected token.
/// @param expected The expected token kind.
/// @param data     Metadata about the bad token.
[[noreturn]]
inline void token_panic(const std::string &expected, const struct Metadata &data){ 
  panic("expected token " + expected, data);
}


/// Panic about a missing match expression.
/// @param data Metadata about the bad token.
[[noreturn]]
inline void match_panic(const struct Metadata &data) { 
  panic("match statement is missing an expression", data);
}


/// Panic about a missing trait implementation.
/// @param expected The name of the expected method.
/// @param data     Metadata about the bad token.
[[noreturn]]
inline void impl_panic(const std::string &expected, const struct Metadata &data) { 
  panic("expected function implementation for " + expected, data); 
}


/// Panic about a duplicate enum variant.
/// @param ident   The variant name.
/// @param enumer  The enum name.
/// @param data    Metadata about the bad token.
[[noreturn]]
inline void enum_variant_panic(const std::string &ident, const std::string &enumer, const struct Metadata &data) {
  panic("enum variant already exists: " + ident + " in " + enumer, data);
}


/// Warn about a statement parsing error.
/// @param msg  The error message.
/// @param data Metadata about the bad input.
/// @return     nullptr
inline std::unique_ptr<Stmt> warn_stmt(std::string msg, const struct Metadata &data) {
  fprintf(stderr, "statimc: warn: %s\nsee: %s:%zu:%zu\n", msg.c_str(), data.filename.c_str(), data.line_n, data.col_n);
  return nullptr;
}


/// Warn about an expression parsing error.
/// @param msg  The error message.
/// @param data Metadata about the bad input.
/// @return     nullptr
inline std::unique_ptr<Expr> warn_expr(std::string msg, const struct Metadata &data) {
  fprintf(stderr, "statimc: warn: %s\nsee: %s:%zu:%zu\n", msg.c_str(), data.filename.c_str(), data.line_n, data.col_n);
  return nullptr;
}


/// Warn about a declaration parsing error.
/// @param msg  The error message.
/// @param data Metadata about the bad input.
/// @return     nullptr
inline std::unique_ptr<Decl> warn_decl(std::string msg, const struct Metadata &data) {
  fprintf(stderr, "statimc: warn: %s\nsee: %s:%zu:%zu\n", msg.c_str(), data.filename.c_str(), data.line_n, data.col_n);
  return nullptr;
}


/// Warn about a function parsing error.
/// @param msg  The error message.
/// @param data Metadata about the bad input.
/// @return     nullptr
inline std::unique_ptr<FunctionDecl> warn_fn(std::string msg, const struct Metadata &data) {
  fprintf(stderr, "statimc: warn: %s\nsee: %s:%zu:%zu\n", msg.c_str(), data.filename.c_str(), data.line_n, data.col_n);
  return nullptr;
}


/// Warn about an enum parsing error.
/// @param msg  The error message.
/// @param data Metadata about the bad input.
/// @return     nullptr
inline std::unique_ptr<EnumDecl> warn_enum(std::string msg, const struct Metadata &data) {
  fprintf(stderr, "statimc: warn: %s\nsee: %s:%zu:%zu\n", msg.c_str(), data.filename.c_str(), data.line_n, data.col_n);
  return nullptr;
}


/// Warn about a struct parsing error.
/// @param msg  The error message.
/// @param data Metadata about the bad input.
/// @return     nullptr
inline std::unique_ptr<StructDecl> warn_struct(std::string msg, const struct Metadata &data) {
  fprintf(stderr, "statimc: warn: %s\nsee: %s:%zu:%zu\n", msg.c_str(), data.filename.c_str(), data.line_n, data.col_n);
  return nullptr;
}


/// Warn about a trait parsing error.
/// @param msg  The error message.
/// @param data Metadata about the bad input.
/// @return     nullptr
inline std::unique_ptr<TraitDecl> warn_trait(std::string msg, const struct Metadata &data) {
  fprintf(stderr, "statimc: warn: %s\nsee: %s:%zu:%zu\n", msg.c_str(), data.filename.c_str(), data.line_n, data.col_n);
  return nullptr;
}


/// Warn about an impl parsing error.
/// @param msg  The error message.
/// @param data Metadata about the bad input.
/// @return     nullptr
inline std::unique_ptr<ImplDecl> warn_impl(std::string msg, const struct Metadata &data) {
  fprintf(stderr, "statimc: warn: %s\nsee: %s:%zu:%zu\n", msg.c_str(), data.filename.c_str(), data.line_n, data.col_n);
  return nullptr;
}


/// Warn about a unit parsing error.
/// @param msg  The error message.
/// @param data Metadata about the bad input.
/// @return     nullptr
inline std::unique_ptr<Unit> warn_unit(std::string msg, const struct Metadata &data) {
  fprintf(stderr, "statimc: warn: %s\nsee: %s:%zu:%zu\n", msg.c_str(), data.filename.c_str(), data.line_n, data.col_n);
  return nullptr;
}


/// Warn about a package unit parsing error.
/// @param msg  The error message.
/// @param data Metadata about the bad input.
/// @return     nullptr
inline std::unique_ptr<PackageUnit> warn_pkg(std::string msg, const struct Metadata &data) {
  fprintf(stderr, "statimc: warn: %s\nsee: %s:%zu:%zu\n", msg.c_str(), data.filename.c_str(), data.line_n, data.col_n);
  return nullptr;
}

#endif  // STATIMC_LOGGER_H
