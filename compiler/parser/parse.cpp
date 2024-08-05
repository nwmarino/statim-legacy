/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <iostream>
#include <memory>
#include <utility>

#include "../core/ast.h"
#include "../core/cctx.h"
#include "../core/token.h"
#include "parser.h"

std::unique_ptr<ProgAST> parse_prog(std::shared_ptr<cctx> ctx) {
  // eat eof
  ctx->tk_next();

  std::vector<std::unique_ptr<FunctionAST>> defs = {};
  while (ctx->prev().kind != TokenKind::Eof) {
    if (ctx->prev().kind == TokenKind::Semi) {
      ctx->tk_next();
      continue;
    }

    if (std::unique_ptr<FunctionAST> func = parse_definition(ctx)) {
      std::cout << "parsed a function definition\n";
      defs.push_back(std::move(func));
    }
  }

  return std::make_unique<ProgAST>(std::move(defs));
}
