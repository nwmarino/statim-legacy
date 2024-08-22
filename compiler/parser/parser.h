#ifndef STATIMC_PARSER_H
#define STATIMC_PARSER_H

/// Parsing function declarations.
/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <memory>

#include "../core/ast.h"
#include "../core/cctx.h"

/// Main parser handling functions.
/// For implementations, see compiler/parser/parser.cpp.

[[nodiscard]]
std::unique_ptr<ProgAST> parse_prog(std::shared_ptr<cctx> ctx);

/// Parse a package.
[[nodiscard]]
std::unique_ptr<PackageAST> parse_package(std::shared_ptr<cctx> ctx);


/// Declaration parsing functions.
/// For implementations, see compiler/parser/decl.cpp.

/// Parse a top level definition.
[[nodiscard]]
std::unique_ptr<AST> parse_toplevel_definition(std::shared_ptr<cctx> ctx);

/// Parse a function definition.
[[nodiscard]]
std::unique_ptr<FunctionAST> parse_definition(std::shared_ptr<cctx> ctx);

/// Parse a function prototype.
[[nodiscard]]
std::unique_ptr<PrototypeAST> parse_prototype(std::shared_ptr<cctx> ctx);

/// Parse a struct definition.
[[nodiscard]]
std::unique_ptr<StructAST> parse_struct(std::shared_ptr<cctx> ctx);

/// Parse an abstract definition.
[[nodiscard]]
std::unique_ptr<AbstractAST> parse_abstract(std::shared_ptr<cctx> ctx);

/// Parse an enum definition.
[[nodiscard]]
std::unique_ptr<EnumAST> parse_enum(std::shared_ptr<cctx> ctx);

/// Parse a variable declaration.
[[nodiscard]]
std::unique_ptr<Statement> parse_var_decl(std::shared_ptr<cctx> ctx);

/// Parse a immutable variable declaration.
[[nodiscard]]
std::unique_ptr<Statement> parse_immut_decl(std::shared_ptr<cctx> ctx);

/// Parse a mutable variable declaration.
[[nodiscard]]
std::unique_ptr<Statement> parse_mut_decl(std::shared_ptr<cctx> ctx);


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

/// Parse an until loop statement.
[[nodiscard]]
std::unique_ptr<Statement> parse_until_stmt(std::shared_ptr<cctx> ctx);

/// Parse a match statement.
[[nodiscard]]
std::unique_ptr<Statement> parse_match_stmt(std::shared_ptr<cctx> ctx);

#endif  // STATIMC_PARSER_H
