/// Copyright 2024 Nick Marino (github.com/nwmarino)

#ifndef STATIMC_PARSE_EXPR_H
#define STATIMC_PARSE_EXPR_H

#include <memory>

#include "ast.h"
#include "tstream.h"

/**
 * Parse a primary expression.
 * 
 * @param t_str Token stream.
 * @returns     Unique ptr to an expression node.
 */
std::unique_ptr<Expr> parse_primary(std::shared_ptr<tstream> t_str);

/**
 * Parse an arbitrary expression.
 * 
 * @param t_str Token stream.
 * @returns     Unique ptr to an expression node.
 */
std::unique_ptr<Expr> parse_expression(std::shared_ptr<tstream> t_str);

/**
 * Parse a numeric literal to an expression node.
 * 
 * @param t_str Token stream.
 * @returns     Unique ptr to an expression node.
 */
std::unique_ptr<Expr> parse_numerical(std::shared_ptr<tstream> t_str);

/**
 * Parse an identifier.
 * 
 * @param t_str Token stream.
 * @returns     Unique ptr to an expression node.
 */
std::unique_ptr<Expr> parse_identifier(std::shared_ptr<tstream> t_str);

/**
 * Parse an argument expression.
 * 
 * @param t_str Token stream.
 * @returns     Unique ptr to an expression node.
 */
std::unique_ptr<Expr> parse_parentheses(std::shared_ptr<tstream> t_str);

/**
 * Parse a binary operation expression.
 * 
 * @param t_str Token stream.
 * @param prec  Precedence of the current operation.
 * @param left  Left side of the operation.
 * @returns     Unique ptr to an expression node.
 */
std::unique_ptr<Expr> parse_binary_op(std::shared_ptr<tstream> t_str, int prec, std::unique_ptr<Expr> left);

/**
 * Get precedence for the next token.
 * 
 * @param t_str Token stream.
 * @return      Relative precendence for future token.
 */
int get_precedence(std::shared_ptr<tstream> t_str);

#endif  // STATIMC_PARSE_EXPR_H