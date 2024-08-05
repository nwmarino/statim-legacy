/// Copyright 2024 Nick Marino (github.com/nwmarino)

#ifndef STATIMC_PARSE_STMT_H
#define STATIMC_PARSE_STMT_H

#include <memory>

#include "ast.h"
#include "tstream.h"

/**
 * Parse a function prototype.
 * 
 * @param t_str Token stream.
 * @returns     Unique ptr to a prototype node.
 */
std::unique_ptr<PrototypeAST> parse_prototype(std::shared_ptr<tstream> t_str);

/**
 * Parse a top-level definition.
 * 
 * @param t_str Token stream.
 * @returns     Unique ptr to a function node.
 */
std::unique_ptr<FunctionAST> parse_toplevel_definition(std::shared_ptr<tstream> t_str);

/**
 * Parse a function definition.
 * 
 * @param t_str Token stream.
 * @returns     Unique ptr to a function node.
 */
std::unique_ptr<FunctionAST> parse_definition(std::shared_ptr<tstream> t_str);

/**
 * Parse a compound statement.
 * 
 * Creates a wrapper for a vector of statements.
 * 
 * @param t_str Token stream.
 * @returns     Unique ptr to a compound statement node.
 */
std::unique_ptr<Statement> parse_compound_statement(std::shared_ptr<tstream> t_str);

/**
 * Parse a return statement.
 * 
 * @param t_str Token stream.
 * @returns     Unique ptr to a return statement node.
 */
std::unique_ptr<Statement> parse_return_statement(std::shared_ptr<tstream> t_str);

#endif  // STATIMC_PARSE_STMT_H