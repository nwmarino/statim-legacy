/// Copyright 2024 Nick Marino (github.com/nwmarino)

#ifndef STATIMC_PARSE_H
#define STATIMC_PARSE_H

#include <memory>

#include "tstream.h"

/**
 * Parse an abstract syntax tree from a token stream.
 * 
 * @param t_str Token stream.
 */
void parse(std::shared_ptr<tstream> t_str);

/**
 * Handle a definition.
 * 
 * @param t_str Token stream.
 */
void handle_definition(std::shared_ptr<tstream> t_str);

/**
 * Handle a top-level definition.
 * 
 * @param t_str Token stream.
 */
void handle_toplevel_definition(std::shared_ptr<tstream> t_str);

#endif  // STATIMC_PARSE_H