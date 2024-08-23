/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <memory>
#include <utility>

#include "parser.h"
#include "../core/cctx.h"
#include "../core/logger.h"
#include "../core/symbol.h"
#include "../core/token.h"

/// Parses an unknown statement.
///
/// A statement is a portion of code that performs some action.
std::unique_ptr<Statement> parse_stmt(std::shared_ptr<cctx> ctx) {
  if (ctx->prev().kind == TokenKind::OpenBrace) {
    return parse_compound_stmt(ctx);
  }

  if (ctx->prev().kind != TokenKind::Identifier) {
    return warn_stmt("unexpected token: " + ctx->prev().value, ctx->prev().meta);
  }
  
  if (ctx->symb_is(ctx->prev().value, SymbolType::Keyword)) {
    KeywordType kw = ctx->symb_get(ctx->prev().value).keyword;
    switch (kw) {
      case KeywordType::Fix: return parse_immut_decl(ctx);
      case KeywordType::If: return parse_if_stmt(ctx);
      case KeywordType::Let: return parse_mut_decl(ctx);
      case KeywordType::Match: return parse_match_stmt(ctx);
      case KeywordType::Return: return parse_return_stmt(ctx);
      case KeywordType::Until: return parse_until_stmt(ctx);
      default: return warn_stmt("unknown keyword: " + ctx->prev().value, ctx->prev().meta);
    }
  }
  return parse_expr(ctx);
}


/// Parses a compound statement. (A list of statements bounded by braces.)
///
/// Compound statements are used to group multiple statements together.
/// They are used in functions, loops, and other control flow structures.
///
/// Certain definitions like structs use braces but not compound statements.
std::unique_ptr<Statement> parse_compound_stmt(std::shared_ptr<cctx> ctx) {
  ctx->tk_next(); // eat opening brace
  std::vector<std::unique_ptr<Statement>> stmts;
  while (ctx->prev().kind != TokenKind::CloseBrace) {
    if (std::unique_ptr<Statement> stmt = parse_stmt(ctx)) {
      stmts.push_back(std::move(stmt));
    }

    if (ctx->prev().kind != TokenKind::Semi) {
      tokexp_panic("';'", std::move(ctx->prev().meta));
    }
    ctx->tk_next(); // eat semi
  }
  ctx->tk_next(); // eat closing brace

  return std::make_unique<CompoundStatement>(std::move(stmts));
}


/// Parse a return statement.
///
/// Return statements appear in the form `return <expr>`.
/// They are used to return a value from a function.
std::unique_ptr<Statement> parse_return_stmt(std::shared_ptr<cctx> ctx) {
  ctx->tk_next(); // eat the `return` identifier

  // implicit void return
  if (ctx->prev().kind == TokenKind::Semi) {
    return std::make_unique<ReturnStatement>(std::make_unique<NullExpr>());
  }
  
  if (std::unique_ptr<Expr> expr = parse_expr(ctx)) {
    return std::make_unique<ReturnStatement>(std::move(expr));
  }

  return warn_stmt("expected expression after 'return'", ctx->prev().meta);
}


/// Parse an if statement.
///
/// If-else statements appear in the form `if <expr> { <stmt> } else { <stmt> }`.
/// They are used to provide control flow based on a condition.
std::unique_ptr<Statement> parse_if_stmt(std::shared_ptr<cctx> ctx) {
  ctx->tk_next(); // eat the `if` identifier
  if (std::unique_ptr<Expr> cond = parse_expr(ctx)) {
    std::unique_ptr<Statement> then_body = parse_stmt(ctx);
    if (!then_body) {
      return warn_stmt("expected statement after 'if' condition", ctx->prev().meta);
    }

    // check for else statement, and parse it accordingly
    if (ctx->symb_is_kw(ctx->prev().value, KeywordType::Else)) {
      ctx->tk_next(); // eat the `else` identifier

      std::unique_ptr<Statement> else_body = parse_stmt(ctx);
      if (!else_body) {
        return warn_stmt("expected statement after 'if' condition", ctx->prev().meta);
      }

      return std::make_unique<IfStatement>(std::move(cond), std::move(then_body), std::move(else_body));
    }

    return std::make_unique<IfStatement>(std::move(cond), std::move(then_body), nullptr);
  }

  return warn_stmt("expected expression after 'if'", ctx->prev().meta);
}


/// Parse an nntil statement.
///
/// Until statements appear in the form `until <expr> { <stmt> }`.
/// They are used to repeat a block of code until a condition is met.
std::unique_ptr<Statement> parse_until_stmt(std::shared_ptr<cctx> ctx) {
  ctx->tk_next(); // eat the `until` identifier

  if (std::unique_ptr<Expr> cond = parse_expr(ctx)) {
    if (std::unique_ptr<Statement> body = parse_stmt(ctx)) {
      return std::make_unique<UntilStatement>(std::move(cond), std::move(body));
    }
  }

  return warn_stmt("expected expression after 'until'", ctx->prev().meta);
}


/// Parse a match statement.
///
/// Match statements appear in the form `match <expr> { case <expr> => <stmt>, ... }`.
/// They are used to match an expression against a series of cases.
std::unique_ptr<Statement> parse_match_stmt(std::shared_ptr<cctx> ctx) {
  ctx->tk_next(); // eat the `match` identifier

  std::unique_ptr<Expr> match_expr = parse_expr(ctx);
  if (!match_expr) {
    return warn_stmt("expected expression after 'match'", ctx->prev().meta);
  }

  if (ctx->prev().kind != TokenKind::OpenBrace) {
    tokexp_panic("'{'", std::move(ctx->prev().meta));
  }
  ctx->tk_next(); // eat opening brace

  std::vector<std::unique_ptr<MatchCase>> cases;
  while (ctx->prev().kind != TokenKind::CloseBrace) {
    std::unique_ptr<Expr> case_expr;
    if (ctx->prev().kind == TokenKind::Identifier && ctx->prev().value == "_") {
      ctx->tk_next(); // eat the default token
      case_expr = std::make_unique<DefaultExpr>();
    } else {
      case_expr = parse_expr(ctx);
    }

    if (!case_expr) {
      return warn_stmt("expected expression after match 'case'", ctx->prev().meta);
    }

    if (ctx->prev().kind != TokenKind::FatArrow) {
      tokexp_panic("'=>'", std::move(ctx->prev().meta));
    }
    ctx->tk_next(); // eat fat arrow

    if (std::unique_ptr<Statement> case_stmt = parse_stmt(ctx)) {
      cases.push_back(std::make_unique<MatchCase>(std::move(case_expr), std::move(case_stmt)));
    }

    if (ctx->prev().kind == TokenKind::CloseBrace) {
      break;
    }

    if (ctx->prev().kind != TokenKind::Comma) {
      tokexp_panic("','", std::move(ctx->prev().meta));
    }
    ctx->tk_next(); // eat comma
  }
  ctx->tk_next(); // eat closing brace

  return std::make_unique<MatchStatement>(std::move(match_expr), std::move(cases));
}
