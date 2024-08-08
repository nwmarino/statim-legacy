/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <iostream>
#include <memory>
#include <utility>

#include "../core/cctx.h"
#include "../core/logger.h"
#include "../core/symbol.h"
#include "../core/token.h"

#include "parser.h"

std::unique_ptr<Statement> parse_stmt(std::shared_ptr<cctx> ctx) {
  if (ctx->prev().kind == TokenKind::OpenBrace) {
    return parse_compound_stmt(ctx);
  }

  if (ctx->prev().kind == TokenKind::Identifier) {
    if (ctx->symb_is(ctx->prev().value, SymbolType::Keyword)) {
      KeywordType kw = ctx->symb_get(ctx->prev().value).keyword;
      switch (kw) {
        case KeywordType::Return:
          return parse_return_stmt(ctx);
        case KeywordType::Fix:
          return parse_immut_decl(ctx);
        case KeywordType::Let:
          return parse_mut_decl(ctx);
        default:
          return warn_stmt("unknown keyword: " + ctx->prev().value);
      }
    }
  }

  return nullptr;
}

std::unique_ptr<Statement> parse_compound_stmt(std::shared_ptr<cctx> ctx) {
  // eat opening block
  ctx->tk_next();

  std::vector<std::unique_ptr<Statement>> stmts = {};
  while (ctx->prev().kind != TokenKind::CloseBrace) {
    if (std::unique_ptr<Statement> stmt = parse_stmt(ctx)) {
      stmts.push_back(std::move(stmt));
    }

    if (ctx->prev().kind != TokenKind::Semi) {
      tokexp_panic("';'", std::move(ctx->prev().meta));
    }

    // eat semi
    ctx->tk_next();

    //return warn_stmt("expected '}' after compound statement");
  }

  // eat closing block
  ctx->tk_next();

  return std::make_unique<CompoundStatement>(std::move(stmts));
}

std::unique_ptr<Statement> parse_return_stmt(std::shared_ptr<cctx> ctx) {
  // eat the return token
  ctx->tk_next();

  if (std::unique_ptr<Expr> expr = parse_expr(ctx)) {
    return std::make_unique<ReturnStatement>(std::move(expr));
  }

  return nullptr;
}
