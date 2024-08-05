/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <memory>

#include "../include/ast.h"
#include "../include/parse.h"
#include "../include/parse_stmt.h"

/**
 * Parse an abstract syntax tree from a token stream.
 * 
 * @param t_str Token stream.
 */
void parse(std::shared_ptr<tstream> t_str)
{
  while (t_str->curr.type != TERMINATE_STREAM) {
    switch (t_str->curr.type) {
			case SEMICOLON:
				t_str->next();
				break;
			case FUNCTION_KEYWORD:
				handle_definition(t_str);
				break;
			default:
				handle_toplevel_definition(t_str);
				break;
		}
  }
}

/**
 * Handle a definition.
 * 
 * @param t_str Token stream.
 */
void handle_definition(std::shared_ptr<tstream> t_str)
{
  if (std::unique_ptr<FunctionAST> FnAST = parse_definition(t_str)) {
	  FnAST->codegen();
  } else {
    t_str->next();
  }
}

/**
 * Handle a top-level definition.
 * 
 * @param t_str Token stream.
 */
void handle_toplevel_definition(std::shared_ptr<tstream> t_str)
{
  if (std::unique_ptr<FunctionAST> FnAST = parse_toplevel_definition(t_str)) {
	  FnAST->codegen();
  } else {
    t_str->next();
  }
}
