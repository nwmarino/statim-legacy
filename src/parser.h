// Copyright (c) 2024 Nick Marino (github.com/nwmarino)

#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "tstream.h"

#include <memory>

void parse(std::shared_ptr<tstream> cc);

#endif  // PARSER_H