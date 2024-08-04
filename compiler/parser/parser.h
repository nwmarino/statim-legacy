#ifndef STATIMC_PARSER_H
#define STATIMC_PARSER_H

/// Parsing function declarations.
/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <memory>

#include "../core/ast.h"
#include "../core/cctx.h"

/// Main parser handling functions.
/// For implementations, see compiler/parser/parser.cpp.


/// Declaration parsing functions.
/// For implementations, see compiler/parser/decl.cpp.

/// Parse a declaration.
[[nodiscard]]
std::unique_ptr<FunctionAST> parse_decl(std::shared_ptr<cctx> ctx);


/// Expression parsing functions.
/// For implementations, see compiler/parser/expr.cpp.

/// Parse an expression of unknown kind.
[[nodiscard]]
std::unique_ptr<Expr> parse_expr(std::shared_ptr<cctx> ctx);

/// Parse a primary expression.
[[nodiscard]]
std::unique_ptr<Expr> parse_primary(std::shared_ptr<cctx> ctx);

/// Parse an identifier.
[[nodiscard]]
std::unique_ptr<Expr> parse_identifier(std::shared_ptr<cctx> ctx);

/// Parse a numerical expression.
[[nodiscard]]
std::unique_ptr<Expr> parse_numerical(std::shared_ptr<cctx> ctx);

/// Parse a boolean expression.
[[nodiscard]]
std::unique_ptr<Expr> parse_boolean(std::shared_ptr<cctx> ctx);

/// Parse a character expression.
[[nodiscard]]
std::unique_ptr<Expr> parse_character(std::shared_ptr<cctx> ctx);

/// Parse a byte expression.
[[nodiscard]]
std::unique_ptr<Expr> parse_byte(std::shared_ptr<cctx> ctx);

/// Parse a string expression.
[[nodiscard]]
std::unique_ptr<Expr> parse_string(std::shared_ptr<cctx> ctx);

/// Parse a byte string expression.
[[nodiscard]]
std::unique_ptr<Expr> parse_bytestring(std::shared_ptr<cctx> ctx);

/// Parse a binary operation expression.
[[nodiscard]]
std::unique_ptr<Expr> parse_binop(std::shared_ptr<cctx> ctx, std::unique_ptr<Expr> lhs, int precedence);

/// Parse a function call expression.
[[nodiscard]]
std::unique_ptr<Expr> parse_function_call(std::shared_ptr<cctx> ctx, const std::string ident);


/// Statement parsing functions.
/// For implementations, see compiler/parser/stmt.cpp.

/// Parse a statement of unknown kind.
[[nodiscard]]
std::unique_ptr<Statement> parse_stmt(std::shared_ptr<cctx> ctx);

/// Parse a compound statement.
[[nodiscard]]
std::unique_ptr<Statement> parse_compound_stmt(std::shared_ptr<cctx> ctx);

/// Parse a return statement.
[[nodiscard]]
std::unique_ptr<Statement> parse_return_stmt(std::shared_ptr<cctx> ctx);

/// Parse an if statement.
[[nodiscard]]
std::unique_ptr<Statement> parse_if_stmt(std::shared_ptr<cctx> ctx);

/// Parse a while statement.
[[nodiscard]]
std::unique_ptr<Statement> parse_while_stmt(std::shared_ptr<cctx> ctx);

/// Parse a break statement.
[[nodiscard]]
std::unique_ptr<Statement> parse_break_stmt(std::shared_ptr<cctx> ctx);

/// Parse a match statement.
[[nodiscard]]
std::unique_ptr<Statement> parse_match_stmt(std::shared_ptr<cctx> ctx);

#endif  // STATIMC_PARSER_H
