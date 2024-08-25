/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <memory>
#include <utility>

#include "parser.h"
#include "../core/ast.h"
#include "../core/cctx.h"
#include "../core/logger.h"
#include "../core/token.h"

/// Parses a package import statement.
///
/// Imports appear in the form of `pkg <identifier>`.
/// The identifier refers to some other file expected by the compiler.
static std::string parse_import(std::shared_ptr<cctx> ctx) {
  ctx->tk_next(); // eat the `pkg` identifier

  ctx->assert_ident();
  std::string import_name = ctx->prev().value;
  ctx->tk_next(); // eat identifier

  return import_name;
}


/// Parses a program as a syntax tree.
///
/// A program is a collection of packages, found by the compiler.
/// Each package is a collection of definitions and imports.
std::unique_ptr<ProgAST> parse_prog(std::shared_ptr<cctx> ctx) {
  ctx->file_next();
  std::vector<std::unique_ptr<PackageAST>> pkgs;
  do {
    if (std::unique_ptr<PackageAST> pkg = parse_package(ctx)) {
      pkgs.push_back(std::move(pkg));
    } else {
      symb_panic("unexpected token: " + ctx->prev().value, ctx->prev().meta);
    }

    ctx->file_next();
  } while (ctx->prev().kind != TokenKind::Eof);

  return std::make_unique<ProgAST>(std::move(pkgs));
}


/// Parses a statim source package.
///
/// A package is a collection of definitions and imports.
/// Definitions include functions, structs, abstracts, enums, and impls.
/// Imports are other packages that are used in the current package.
std::unique_ptr<PackageAST> parse_package(std::shared_ptr<cctx> ctx) {
  std::string name = ctx->filename().substr(0, ctx->filename().find_last_of('.'));

  std::vector<std::unique_ptr<AST>> defs;
  std::vector<std::string> imports;
  while (ctx->prev().kind != TokenKind::Eof) {
    ctx->assert_ident(); // verify any leading token is an ident

    // parse definition based on keyword
    if (ctx->symb_is(ctx->prev().value, SymbolType::Keyword)) {
      KeywordType kw = ctx->symb_get(ctx->prev().value).keyword;
      switch (kw) {
        case KeywordType::Abstract:
          if (std::unique_ptr<AbstractAST> abstr = parse_abstract(ctx)) {
            defs.push_back(std::move(abstr));
          }
          break;
        case KeywordType::Pkg:
          imports.push_back(parse_import(ctx));
          break;
        case KeywordType::Fn:
          if (std::unique_ptr<FunctionAST> func = parse_definition(ctx)) {
            defs.push_back(std::move(func));
          }
          break;
        case KeywordType::Struct:
          if (std::unique_ptr<StructAST> struc = parse_struct(ctx)) {
            defs.push_back(std::move(struc));
          }
          break;
        
        case KeywordType::Enum:
          if (std::unique_ptr<EnumAST> enm = parse_enum(ctx)) {
            defs.push_back(std::move(enm));
          }
          break;
        case KeywordType::Impl:
          if (std::unique_ptr<ImplAST> impl = parse_impl(ctx)) {
            defs.push_back(std::move(impl));
          }
          break;
        default:
          symb_panic("unknown keyword: " + ctx->prev().value, ctx->prev().meta);
      }
    } else {
      symb_panic("unresolved token: " + ctx->prev().value, ctx->prev().meta);
    }

    // eat possible semi
    if (ctx->prev().kind == TokenKind::Semi) {
      ctx->tk_next();
    }
  }

  return std::make_unique<PackageAST>(name, std::move(defs), std::move(imports));
}
