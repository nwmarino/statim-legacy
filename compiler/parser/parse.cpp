/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <iostream>
#include <memory>
#include <utility>

#include "../core/ast.h"
#include "../core/cctx.h"
#include "../core/token.h"
#include "../core/logger.h"
#include "parser.h"

std::unique_ptr<ProgAST> parse_prog(std::shared_ptr<cctx> ctx) {
  // eat eof
  ctx->tk_next();

  std::vector<std::unique_ptr<PackageAST>> pkgs = {};
  while (ctx->prev().kind != TokenKind::Eof) {
    break;
    if (ctx->prev().kind == TokenKind::Semi) {
      ctx->tk_next();
      continue;
    }
  }

  return std::make_unique<ProgAST>(std::move(pkgs));
}


std::unique_ptr<PackageAST> parse_package(std::shared_ptr<cctx> ctx) {
  std::string name = ctx->filename();

  std::vector<std::unique_ptr<AST>> defs = {};
  while (ctx->prev().kind != TokenKind::Eof) {
    if (ctx->prev().kind == TokenKind::Semi) {
      ctx->tk_next();
      continue;
    }

    if (ctx->prev().kind != TokenKind::Identifier) {
      tokexp_panic("identifier", ctx->prev().meta);
    }

    if (ctx->symb_is_kw(ctx->prev().value, KeywordType::Fn)) {
      if (std::unique_ptr<FunctionAST> func = parse_definition(ctx)) {
        defs.push_back(std::move(func));
      }
    } else if (ctx->symb_is_kw(ctx->prev().value, KeywordType::Struct)) {
      if (std::unique_ptr<StructAST> struc = parse_struct(ctx)) {
        defs.push_back(std::move(struc));
      }
    } else if (ctx->symb_is_kw(ctx->prev().value, KeywordType::Abstract)) {
      if (std::unique_ptr<AbstractAST> abstr = parse_abstract(ctx)) {
        defs.push_back(std::move(abstr));
      }
    } else if (ctx->symb_is_kw(ctx->prev().value,KeywordType::Enum)) {
      if (std::unique_ptr<EnumAST> enm = parse_enum(ctx)) {
        defs.push_back(std::move(enm));
      }
    } else {
      symb_panic("unexpected token: " + ctx->prev().value, ctx->prev().meta);
    }
  }
}
