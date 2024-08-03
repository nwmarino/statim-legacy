/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <memory>
#include <utility>

#include "../core/logger.h"
#include "../core/token.h"
#include "parse_expr.h"

/**
 * Parse a primary expression.
 * 
 * @param t_str Token stream.
 * @returns     Unique ptr to an expression node.
 */
std::unique_ptr<Expr> parse_primary(std::shared_ptr<tstream> t_str)
{
  switch (t_str->curr.type)
  {
    case IDENTIFIER:
      return parse_identifier(t_str);
    case C_INT:
    case C_FP:
      return parse_numerical(t_str);
    case SET_PAREN:
      return parse_parentheses(t_str);
    default:
      return logError("Unable to resolve token of type " + std::to_string(t_str->curr.type));
  }
}

/**
 * Parse an arbitrary expression.
 * 
 * @param t_str Token stream.
 * @returns     Unique ptr to an expression node.
 */
std::unique_ptr<Expr> parse_expression(std::shared_ptr<tstream> t_str)
{
  if (std::unique_ptr<Expr> base = parse_primary(t_str))
    return parse_binary_op(t_str, 0, std::move(base));
  return nullptr;
}

/**
 * Parse a numeric literal to an expression node.
 * 
 * @param t_str Token stream.
 * @returns     Unique ptr to an expression node.
 */
std::unique_ptr<Expr> parse_numerical(std::shared_ptr<tstream> t_str)
{
  TokenType numerType = t_str->curr.type;
  std::string value = t_str->curr.value;
  t_str->next(); // eat literal

  if (numerType == C_INT)
    return std::make_unique<IntegerExpr>(stoll(value));
  else if (numerType == C_FP)
    return std::make_unique<FloatingPointExpr>(stod(value));
  else
    return nullptr;
}

/**
 * Parse an identifier.
 * 
 * @param t_str Token stream.
 * @returns     Unique ptr to an expression node.
 */
std::unique_ptr<Expr> parse_identifier(std::shared_ptr<tstream> t_str)
{
  std::string id = t_str->curr.value;

  t_str->next(); // eat identifier

  // check parentheses for function call
  if (t_str->curr.type != SET_PAREN)
    return nullptr; // parse as variable

  t_str->next(); // eat open parentheses
  std::vector<std::unique_ptr<Expr>> args;
  while (t_str->curr.type != END_PAREN) {
    if (std::unique_ptr<Expr> arg = parse_expression(t_str))
      args.push_back(std::move(arg));

    if (t_str->curr.type != COMMA)
      return logError("Expected separator between function call arguments.");

    t_str->next(); // eat comma separator
  }
  t_str->next(); // eat close parentheses
  return std::make_unique<FunctionCallExpr>(id, std::move(args));
}

/**
 * Parse an argument expression.
 * 
 * @param t_str Token stream.
 * @returns     Unique ptr to an expression node.
 */
std::unique_ptr<Expr> parse_parentheses(std::shared_ptr<tstream> t_str)
{
  t_str->next(); // eat open parentheses

  std::unique_ptr<Expr> exp = parse_expression(t_str);
  if (!exp)
    return nullptr;

  if (t_str->curr.type != END_PAREN)
    return logError("Unmatched opening parentheses.");

  t_str->next(); // eat close parentheses
  return exp;
}

/**
 * Parse a binary operation expression.
 * 
 * @param t_str Token stream.
 * @param prec  Precedence of the current operation.
 * @param left  Left side of the operation.
 * @returns     Unique ptr to an expression node.
 */
std::unique_ptr<Expr> parse_binary_op(std::shared_ptr<tstream> t_str, int prec, std::unique_ptr<Expr> left)
{
  while (true) {
    int token_prec = get_precedence(t_str);

    if (token_prec < prec)
      return left;

    TokenType opType = t_str->curr.type;
    t_str->next(); // eat operator

    std::unique_ptr<Expr> right = parse_primary(t_str);
    if (!right)
      return nullptr;

    int next_prec = get_precedence(t_str);
    if (token_prec < next_prec) {
      right = parse_binary_op(t_str, token_prec + 1, std::move(right));
      if (!right)
        return nullptr;
    }

    left = std::make_unique<BinaryExpr>(opType, std::move(left), std::move(right));
  }
}

/**
 * Get precedence for the next token.
 * 
 * @param t_str Token stream.
 * @return      Relative precendence for future token.
 */
int get_precedence(std::shared_ptr<tstream> t_str)
{
  return -1;
}
