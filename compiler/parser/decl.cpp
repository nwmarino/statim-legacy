/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <memory>
#include <utility>

#include "parser.h"
#include "../core/ast.h"
#include "../core/cctx.h"
#include "../core/logger.h"
#include "../core/symbol.h"

std::unique_ptr<AST> parse_toplevel_definition(std::shared_ptr<cctx> ctx) {
  if (ctx->prev().kind == TokenKind::Identifier && ctx->symb_is_kw(ctx->prev().value, KeywordType::Struct)) {
    return parse_struct(ctx);
  }
  return nullptr;
}


std::unique_ptr<FunctionAST> parse_definition(std::shared_ptr<cctx> ctx) {
  // eat fn keyword
  ctx->tk_next();

  if (ctx->prev().kind != TokenKind::Identifier) {
    tokexp_panic("identifier", ctx->prev().meta);
  }

  std::unique_ptr<PrototypeAST> head = parse_prototype(ctx);

  if (!head)
    return nullptr;

  if (std::unique_ptr<Statement> body = parse_stmt(ctx)) {
    return std::make_unique<FunctionAST>(std::move(head), std::move(body));
  }

  return nullptr;
}


std::unique_ptr<PrototypeAST> parse_prototype(std::shared_ptr<cctx> ctx) {
  const std::string name = ctx->prev().value;
  if (ctx->symb_exists(name)) {
    symb_decl_panic(name, ctx->prev().meta);
  }

  // eat function identifier
  ctx->tk_next();

  if (ctx->prev().kind != TokenKind::OpenParen) {
    tokexp_panic("'('", ctx->prev().meta);
  }

  // eat open parentheses
  ctx->tk_next();

  // parse function arguments
  std::vector<std::pair<std::string, std::string>> args;
  while (ctx->prev().kind == TokenKind::Identifier) {
    std::string argName = ctx->prev().value;

    // eat the argument name
    ctx->tk_next();

    if (ctx->prev().kind != TokenKind::Colon) {
      tokexp_panic("':'", ctx->prev().meta);
    }

    // eat the colon
    ctx->tk_next();

    if (ctx->prev().kind != TokenKind::Identifier) {
      tokexp_panic("identifier", ctx->prev().meta);
    }
    std::string argTy = ctx->prev().value;

    // eat the argument type
    ctx->tk_next();

    args.push_back(std::make_pair(argName, argTy));

    if (ctx->prev().kind == TokenKind::Comma) {
      ctx->tk_next();
    }
  }

  if (ctx->prev().kind != TokenKind::CloseParen) {
    tokexp_panic("')'", ctx->prev().meta);
  }

  // eat close parentheses
  ctx->tk_next();

  // check main does not have arguments
  if (name == "main" && args.size() != 0) {
    return warn_proto("entry function 'main' should not possess arguments");
  }

  // parse return type
  std::string ret_ty;
  if (name != "main") {
    if (ctx->prev().kind != TokenKind::Arrow) {
    tokexp_panic("'->'", ctx->prev().meta);
    }

    // eat arrow
    ctx->tk_next();

    if (ctx->prev().kind != TokenKind::Identifier) {
      tokexp_panic("identifier", ctx->prev().meta);
    }

    ret_ty = ctx->prev().value;
    if (!ctx->symb_exists(ret_ty)) {
      symb_type_panic(ret_ty, ctx->prev().meta);
    }

    // eat return type
    ctx->tk_next();
  } else {
    ret_ty = "void";
  }

  return std::make_unique<PrototypeAST>(name, std::move(args), ret_ty);
}


std::unique_ptr<StructAST> parse_struct(std::shared_ptr<cctx> ctx) {
  // eat struct keyword
  ctx->tk_next();

  if (ctx->prev().kind != TokenKind::Identifier) {
    tokexp_panic("identifier", ctx->prev().meta);
  }

  const std::string name = ctx->prev().value;
  if (ctx->symb_exists(name)) {
    symb_decl_panic(name, ctx->prev().meta);
  }

  // eat struct identifier
  ctx->tk_next();

  if (ctx->prev().kind != TokenKind::OpenBrace) {
    tokexp_panic("'{'", ctx->prev().meta);
  }

  // eat open brace
  ctx->tk_next();

  std::vector<std::pair<std::string, std::string>> fields;
  while (ctx->prev().kind == TokenKind::Identifier) {
    std::string field_name = ctx->prev().value;

    // eat field name
    ctx->tk_next();

    if (ctx->prev().kind != TokenKind::Colon) {
      tokexp_panic("':'", ctx->prev().meta);
    }

    // eat colon
    ctx->tk_next();

    if (ctx->prev().kind != TokenKind::Identifier) {
      tokexp_panic("identifier", ctx->prev().meta);
    }

    std::string field_ty = ctx->prev().value;

    // eat field type
    ctx->tk_next();

    fields.push_back(std::make_pair(field_name, field_ty));

    if (ctx->prev().kind == TokenKind::CloseBrace) {
      break;
    }

    if (ctx->prev().kind != TokenKind::Comma) {
      tokexp_panic("','", ctx->prev().meta);
    }

    // eat the comma
    ctx->tk_next();
  }

  if (ctx->prev().kind != TokenKind::CloseBrace) {
    tokexp_panic("'}'", ctx->prev().meta);
  }

  // eat close brace
  ctx->tk_next();

  return std::make_unique<StructAST>(name, std::move(fields));
}


std::unique_ptr<AbstractAST> parse_abstract(std::shared_ptr<cctx> ctx) {
  // eat abstract keyword
  ctx->tk_next();

  if (ctx->prev().kind != TokenKind::Identifier) {
    tokexp_panic("identifier", ctx->prev().meta);
  }
  std::string name = ctx->prev().value;

  // eat abstract identifier
  ctx->tk_next();

  if (ctx->prev().kind != TokenKind::OpenBrace) {
    tokexp_panic("'{'", ctx->prev().meta);
  }

  // eat open brace
  ctx->tk_next();

  std::vector<std::unique_ptr<PrototypeAST>> methods;
  while (ctx->prev().kind != TokenKind::CloseBrace) {
    if (ctx->symb_is_kw(ctx->prev().value, KeywordType::Fn)) {
      // eat the fn keyword
      ctx->tk_next();

      if (std::unique_ptr<PrototypeAST> method = parse_prototype(ctx)) {
        methods.push_back(std::move(method));
      }
    }

    if (ctx->prev().kind == TokenKind::CloseBrace) {
      break;
    }

    if (ctx->prev().kind != TokenKind::Semi) {
      tokexp_panic("';'", ctx->prev().meta);
    }

    // eat the semi
    ctx->tk_next();
  }

  if (ctx->prev().kind != TokenKind::CloseBrace) {
    tokexp_panic("'}'", ctx->prev().meta);
  }

  // eat close brace
  ctx->tk_next();

  return std::make_unique<AbstractAST>(name, std::move(methods));
}


std::unique_ptr<Statement> parse_var_decl(std::shared_ptr<cctx> ctx) {
  switch (ctx->symb_get(ctx->prev().value).keyword) {
    case KeywordType::Fix:
      return parse_immut_decl(ctx);

    case KeywordType::Let:
      return parse_mut_decl(ctx);

    default:
      return nullptr;
  }
  return warn_stmt("expected variable declaration");
}


std::unique_ptr<Statement> parse_immut_decl(std::shared_ptr<cctx> ctx) {
  // eat the fix token
  ctx->tk_next();

  // expect identifier to proceed fix keyword
  if (ctx->prev().kind != TokenKind::Identifier) {
    tokexp_panic("identifier", ctx->prev().meta);
  }

  // check variable value does not yet exist
  const std::string name = ctx->prev().value;
  if (ctx->symb_exists(name)) {
    symb_decl_panic(name, ctx->prev().meta);
  }
  
  // save metadata
  struct Metadata meta = ctx->prev().meta;
  ctx->symb_add(name, Symbol(SymbolType::Constant, ctx->prev().meta));

  // eat the identifier
  ctx->tk_next();

  // expect type separator
  if (ctx->prev().kind != TokenKind::Colon) {
    tokexp_panic("':'", ctx->prev().meta);
  }

  // eat the colon
  ctx->tk_next();

  // parse the type
  const std::string ty = ctx->prev().value;
  if (ctx->prev().kind != TokenKind::Identifier) {
    tokexp_panic("identifier", ctx->prev().meta);
  }

  /* no types as of yet

  // check type exists
  if (!ctx->symb_get(ty)) {
      symb_type_panic(ty, std::move(ctx->prev()->meta));
  }
  
  */

  // eat the type decl
  ctx->tk_next();

  // expect assignment operator
  if (ctx->prev().kind != TokenKind::Eq) {
    tokexp_panic("'='", ctx->prev().meta);
  }

  // eat the assignment operator
  ctx->tk_next();

  // parse the required expression
  if (std::unique_ptr<Expr> expr = parse_expr(ctx)) {
    ctx->symb_add(name, Symbol(SymbolType::Constant, meta));
    return std::make_unique<AssignmentStatement>(name, ty, std::move(expr));
  }

  return warn_const("expected expression after immutable declaration: " + name);
}


std::unique_ptr<Statement> parse_mut_decl(std::shared_ptr<cctx> ctx) {
  // eat let token
  ctx->tk_next();

  // expect identifier to proceed let keyword
  if (ctx->prev().kind != TokenKind::Identifier) {
    tokexp_panic("identifier", ctx->prev().meta);
  }

  // check variable value does not yet exist
  const std::string name = ctx->prev().value;
  if (ctx->symb_exists(name)) {
    symb_decl_panic(name, ctx->prev().meta);
  }

  // save metadata
  struct Metadata meta = ctx->prev().meta;

  // eat the identifier
  ctx->tk_next();

  // expect type separator
  if (ctx->prev().kind != TokenKind::Colon) {
    tokexp_panic("':'", ctx->prev().meta);
  }

  // eat the colon
  ctx->tk_next();

  // parse the type
  const std::string ty = ctx->prev().value;
  if (ctx->prev().kind != TokenKind::Identifier) {
    tokexp_panic("identifier", ctx->prev().meta);
  }

  // eat the type decl
  ctx->tk_next();

  // if no declaration
  if (ctx->prev().kind == TokenKind::Semi) {
    ctx->symb_add(name, Symbol(SymbolType::Variable, meta));
    return std::make_unique<AssignmentStatement>(name, ty, nullptr);
  }

  // expect assignment operator
  if (ctx->prev().kind != TokenKind::Eq) {
    tokexp_panic("'='", ctx->prev().meta);
  }

  // eat the assignment operator
  ctx->tk_next();

  // parse the required expression
  if (std::unique_ptr<Expr> expr = parse_expr(ctx)) {
    ctx->symb_add(name, Symbol(SymbolType::Variable, meta));
    return std::make_unique<AssignmentStatement>(name, ty, std::move(expr));
  }

  return nullptr;
}
