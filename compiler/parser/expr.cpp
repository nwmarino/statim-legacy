/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <iostream>
#include <memory>
#include <utility>

#include "../core/ast.h"
#include "../core/cctx.h"
#include "../core/logger.h"
#include "../core/symbol.h"
#include "../core/token.h"
#include "parser.h"

/// Get the precedence of a binary operator.
static int get_precedence(TokenKind op) {
  switch (op)
  {
    case TokenKind::Increment:
    case TokenKind::Decrement:
      return 11;

    case TokenKind::Dot:
      return 10;

    case TokenKind::Not:
    case TokenKind::Hash:
    case TokenKind::At:
    case TokenKind::Range:
      return 9;

    case TokenKind::Star:
    case TokenKind::Slash:
      return 8;

    case TokenKind::Add:
    case TokenKind::Sub:
      return 7;

    case TokenKind::LeftShift:
    case TokenKind::RightShift:
      return 6;

    case TokenKind::LessThan:
    case TokenKind::LessThanEq:
    case TokenKind::GreaterThan:
    case TokenKind::GreaterThanEq:
      return 5;

    case TokenKind::EqEq:
    case TokenKind::NotEq:
      return 4;

    case TokenKind::And:
    case TokenKind::Or:
    case TokenKind::Xor:
      return 3;

    case TokenKind::AndAnd:
    case TokenKind::OrOr:
    case TokenKind::XorXor:
      return 2;

    case TokenKind::Eq:
    case TokenKind::AddEq:
    case TokenKind::SubEq:
    case TokenKind::StarEq:
    case TokenKind::SlashEq:
      return 1;
    
    default:
      return 0;
  }
  return -1;
}


std::unique_ptr<Expr> parse_expr(std::shared_ptr<cctx> ctx) {
  if (std::unique_ptr<Expr> base = parse_primary(ctx)) {
    return parse_binop(ctx, std::move(base), 0);
  }

  return nullptr;
}


std::unique_ptr<Expr> parse_primary(std::shared_ptr<cctx> ctx) {
  switch (ctx->prev().lit_kind) {
    case LiteralKind::Integer:
    case LiteralKind::Float:
      return parse_numerical(ctx);

    case LiteralKind::Char:
    case LiteralKind::Byte:
      return parse_character(ctx);

    case LiteralKind::String:
    case LiteralKind::ByteString:
      return parse_string(ctx);

    case LiteralKind::Bool:
      return parse_boolean(ctx);

    case LiteralKind::Null:
      // eat the null token
      ctx->tk_next();
      return std::make_unique<NullExpr>();
  }
  
  if (ctx->prev().kind == TokenKind::Identifier) {
    return parse_identifier(ctx);
  }

  return warn_expr("unknown primary expression kind: " + std::to_string(ctx->prev().lit_kind));
}


std::unique_ptr<Expr> parse_identifier(std::shared_ptr<cctx> ctx) {
  const std::string ident = ctx->prev().value;

  // eat the identifier token
  ctx->tk_next();

  if (ctx->prev().kind == TokenKind::OpenParen) {
    return parse_function_call(ctx, ident);
  }

  if (!ctx->symb_is(ident, SymbolType::Constant) && !ctx->symb_is(ident, SymbolType::Variable)) {
    symb_var_panic(ident, std::move(ctx->prev().meta));
  }

  return std::make_unique<VariableExpr>(ident);
}


std::unique_ptr<Expr> parse_numerical(std::shared_ptr<cctx> ctx) {
  LiteralKind kind = ctx->prev().lit_kind;
  std::string value = ctx->prev().value;

  // eat the literal token
  ctx->tk_next();

  if (kind == LiteralKind::Integer)
    return std::make_unique<IntegerExpr>(std::stoll(value));
  if (kind == LiteralKind::Float)
    return std::make_unique<FloatingPointExpr>(std::stod(value));

  return warn_expr("unknown literal kind: " + std::to_string(kind));
}


std::unique_ptr<Expr> parse_boolean(std::shared_ptr<cctx> ctx) {
  std::string value = ctx->prev().value;

  // check bad token
  if (value != "true" && value != "false") {
    sc_panic("invalid boolean token: " + value, std::move(ctx->prev().meta));
  }

  // eat the bool token
  ctx->tk_next();

  return std::make_unique<BoolExpr>(value == "true");
}


std::unique_ptr<Expr> parse_character(std::shared_ptr<cctx> ctx) {
  char value = ctx->prev().value[0];

  // eat the char token
  ctx->tk_next();

  return std::make_unique<CharExpr>(value);
}


std::unique_ptr<Expr> parse_byte(std::shared_ptr<cctx> ctx) {
  char value = ctx->prev().value[0];

  // eat the byte token
  ctx->tk_next();

  return std::make_unique<ByteExpr>(value);
}


std::unique_ptr<Expr> parse_string(std::shared_ptr<cctx> ctx) {
  std::string value = ctx->prev().value;

  // eat the string token
  ctx->tk_next();

  return std::make_unique<StringExpr>(value);
}


std::unique_ptr<Expr> parse_bytestring(std::shared_ptr<cctx> ctx) {
  std::string value = ctx->prev().value;

  // eat the bytestring token
  ctx->tk_next();

  return std::make_unique<ByteStringExpr>(value);
}


std::unique_ptr<Expr> parse_binop(std::shared_ptr<cctx> ctx, std::unique_ptr<Expr> lhs, int precedence) {
  while (true) {
    int token_prec = get_precedence(ctx->prev().kind);

    if (token_prec < precedence)
      return lhs;

    TokenKind op_kind = ctx->prev().kind;

    // eat operator
    ctx->tk_next();

    std::unique_ptr<Expr> rhs = parse_primary(ctx);
    if (!rhs)
      return nullptr;

    int next_prec = get_precedence(ctx->prev().kind);
    if (token_prec < next_prec) {
      rhs = parse_binop(ctx, std::move(rhs), token_prec + 1);
      if (!rhs)
        return nullptr;
    }

    lhs = std::make_unique<BinaryExpr>(op_kind, std::move(lhs), std::move(rhs));
  }
}


std::unique_ptr<Expr> parse_function_call(std::shared_ptr<cctx> ctx, const std::string ident) {
  // verify that ident is a parsed function
  if (ctx->symb_is(ident, SymbolType::Function)) {
    symb_func_panic(ident, std::move(ctx->prev().meta));
  }

  // eat the open paren token
  ctx->tk_next();

  // parse call arguments
  std::vector<std::unique_ptr<Expr>> args;
  while (ctx->prev().kind != TokenKind::CloseParen) {
    if (std::unique_ptr<Expr> arg = parse_expr(ctx))
      args.push_back(std::move(arg));

    if (ctx->prev().kind == TokenKind::CloseParen)
      break;

    if (ctx->prev().kind != TokenKind::Comma)
      tokexp_panic("','", std::move(ctx->prev().meta));

    // eat arg separator
    ctx->tk_next(); 
  }

  // eat the close paren token
  ctx->tk_next();
  return std::make_unique<FunctionCallExpr>(ident, std::move(args));
}
