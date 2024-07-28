/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <memory>
#include <string>
#include <utility>

#include "../include/ast.h"
#include "../include/logger.h"
#include "../include/parse_expr.h"
#include "../include/parse_stmt.h"
#include "../include/token.h"
#include "../include/tstream.h"

/**
 * Parse a function prototype.
 * 
 * @param t_str Token stream.
 * @returns     Unique ptr to a prototype node.
 */
std::unique_ptr<PrototypeAST> parse_prototype(std::shared_ptr<tstream> t_str)
{
  if (t_str->curr.type != IDENTIFIER)
    return logErrorPr("Function name must proceed definition.");

  std::string name = t_str->curr.value;
  t_str->next(); // eat identifier

  if (t_str->curr.type != SET_PAREN)
    return logErrorPr("Expected opening parentheses proceeding function identifier.");

  t_str->next(); // eat open parentheses

  std::vector<std::string> argNames;
  while (t_str->curr.type == IDENTIFIER) {
    argNames.push_back(t_str->curr.value);
    t_str->next(); // eat current identifier
  }

  if (t_str->curr.type != END_PAREN)
    return logErrorPr("Expected closing parentheses proceeding function arguments.");

  t_str->next(); // eat close parentheses
  t_str->next(); // eat arrow

  RetType retType;
  switch (t_str->curr.type) {
    case INT_KEYWORD: retType = RT_INT; break;
    case FLOAT_KEYWORD: retType = RT_FLOAT; break;
    case BOOL_KEYWORD: retType = RT_BOOL; break;
    case CHAR_KEYWORD: retType = RT_CHAR; break;
    case STRING_KEYWORD: retType = RT_STRING; break;
    default:
      return logErrorPr("Function " + name + " missing return type.");
  }

  t_str->next(); // eat return type keyword
  return std::make_unique<PrototypeAST>(name, std::move(argNames), retType);
}

/**
 * Parse a top-level definition.
 * 
 * @param t_str Token stream.
 * @returns     Unique ptr to a function node.
 */
std::unique_ptr<FunctionAST> parse_toplevel_definition(std::shared_ptr<tstream> t_str)
{
  if (std::unique_ptr<Statement> body = parse_compound_statement(t_str)) {
    auto head = std::make_unique<PrototypeAST>("", std::vector<std::string>(), RT_VOID);
    return std::make_unique<FunctionAST>(std::move(head), std::move(body));
  }
  return nullptr;
}

/**
 * Parse a function definition.
 * 
 * @param t_str Token stream.
 * @returns     Unique ptr to a function node.
 */
std::unique_ptr<FunctionAST> parse_definition(std::shared_ptr<tstream> t_str)
{
  t_str->next(); // eat function keyword

  std::unique_ptr<PrototypeAST> head = parse_prototype(t_str);

  if (!head)
    return nullptr;

  //printf("%d", t_str->curr.type);

  if (std::unique_ptr<Statement> body = parse_compound_statement(t_str))
    return std::make_unique<FunctionAST>(std::move(head), std::move(body));

  return nullptr;
}

/**
 * Parse a compound statement.
 * 
 * Creates a wrapper for a vector of statements.
 * 
 * @param t_str Token stream.
 * @returns     Unique ptr to a compound statement node.
 */
std::unique_ptr<Statement> parse_compound_statement(std::shared_ptr<tstream> t_str)
{
  if (t_str->curr.type != SET_BLOCK)
    return logErrorS("Expected '{' before function body.");

  t_str->next(); // eat opening block
  std::vector<std::unique_ptr<Statement>> stmts;
  while (t_str->curr.type != END_BLOCK) {
    if (t_str->curr.type == RETURN_KEYWORD) {
      std::unique_ptr<Statement> stmt = parse_return_statement(t_str);
      stmts.push_back(std::move(stmt));
      continue;
    }

    return logErrorS("Expected '}' after function body.");
  }
  
  t_str->next(); // eat end block
  return std::make_unique<CompoundStatement>(std::move(stmts));
}

/**
 * Parse a return statement.
 * 
 * @param t_str Token stream.
 * @returns     Unique ptr to a return statement node.
 */
std::unique_ptr<Statement> parse_return_statement(std::shared_ptr<tstream> t_str)
{
  t_str->next(); // eat return keyword

  if (std::unique_ptr<Expr> expr = parse_expression(t_str)) {
    if (t_str->curr.type != SEMICOLON)
      return logErrorS("Expected ';' after return statement.");

    t_str->next(); // eat semicolon
    return std::make_unique<ReturnStatement>(std::move(expr));
  }

  return nullptr;
}
