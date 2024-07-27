/// Copyright 2024 Nick Marino (github.com/nwmarino)

#ifndef STATIMC_PARSER_H
#define STATIMC_PARSER_H

#include "container.h"
#include "tstream.h"

#include <memory>

/**
 * Parse an abstract syntax tree from a token stream.
 * 
 * @param container LLVM dependency container.
 * @param cc        The token stream to parse through.
 */
void parse(std::shared_ptr<LLContainer> container, std::shared_ptr<tstream> cc);

#endif  // STAITMC_PARSER_H
