// Copyright (c) 2024 Nick Marino (github.com/nwmarino)

#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "tstream.h"

void parse(tstream cc);
std::unique_ptr<Expr> parseBinOp(tstream cc, int expPrec,
                                        std::unique_ptr<Expr> lhs);
int getPrecedence(tstream cc);
std::unique_ptr<Expr> parsePrimary(tstream cc);

#endif  // PARSER_H