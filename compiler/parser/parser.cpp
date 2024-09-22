/// This source file houses the main recursive descent parsing functions for the AST builder.

#include <memory>

#include "../include/ast/Builder.h"
#include "../include/ast/Decl.h"
#include "../include/ast/Expr.h"
#include "../include/ast/Stmt.h"
#include "../include/core/ASTContext.h"
#include "../include/core/Logger.h"
#include "../include/core/Utils.h"

static std::shared_ptr<Scope> curr_scope;

static std::unique_ptr<Expr> parse_expr(std::unique_ptr<ASTContext> &ctx);
static std::unique_ptr<Stmt> parse_stmt(std::unique_ptr<ASTContext> &ctx);
static std::unique_ptr<Stmt> parse_var_decl(std::unique_ptr<ASTContext> &ctx);
static std::unique_ptr<Expr> parse_primary_expr(std::unique_ptr<ASTContext> &ctx);

static UnaryOp get_unary_op(TokenKind op) {
  switch (op) {
    case TokenKind::Not:
      return UnaryOp::Bang;
    case TokenKind::Hash:
      return UnaryOp::Rune;
    case TokenKind::At:
      return UnaryOp::Ref;
    case TokenKind::Dot:
      return UnaryOp::Access;
    default:
      return UnaryOp::UnknownUnaryOp;
  }
}


static BinaryOp get_binary_op(TokenKind op) {
  switch (op) {
    case TokenKind::Eq:
      return BinaryOp::Assign;
    case TokenKind::AddEq:
      return BinaryOp::AddAssign;
    case TokenKind::SubEq:
      return BinaryOp::SubAssign;
    case TokenKind::StarEq:
      return BinaryOp::StarAssign;
    case TokenKind::SlashEq:
      return BinaryOp::SlashAssign;
    case TokenKind::EqEq:
      return BinaryOp::IsEq;
    case TokenKind::NotEq:
      return BinaryOp::IsNotEq;
    case TokenKind::AndAnd:
      return BinaryOp::LogicAnd;
    case TokenKind::OrOr:
      return BinaryOp::LogicOr;
    case TokenKind::LessThan:
      return BinaryOp::Lt;
    case TokenKind::LessThanEq:
      return BinaryOp::LtEquals;
    case TokenKind::GreaterThan:
      return BinaryOp::Gt;
    case TokenKind::GreaterThanEq:
      return BinaryOp::GtEquals;
    case TokenKind::Add:
      return BinaryOp::Plus;
    case TokenKind::Sub:
      return BinaryOp::Minus;
    case TokenKind::Star:
      return BinaryOp::Mult;
    case TokenKind::Slash:
      return BinaryOp::Div;
    default:
      return BinaryOp::UnknownBinaryOp;
  }
}


/// Get the precedence of an operator.
static int get_precedence(TokenKind op) {
  switch (op) {
    case TokenKind::Dot:
      return 8;
    case TokenKind::Not:
    case TokenKind::Hash:
    case TokenKind::At:
      return 7;
    case TokenKind::Star:
    case TokenKind::Slash:
      return 6;
    case TokenKind::Add:
    case TokenKind::Sub:
      return 5;
    case TokenKind::LessThan:
    case TokenKind::LessThanEq:
    case TokenKind::GreaterThan:
    case TokenKind::GreaterThanEq:
      return 4;
    case TokenKind::EqEq:
    case TokenKind::NotEq:
      return 3;
    case TokenKind::AndAnd:
    case TokenKind::OrOr:
      return 2;
    case TokenKind::Eq:
    case TokenKind::AddEq:
    case TokenKind::SubEq:
    case TokenKind::StarEq:
    case TokenKind::SlashEq:
      return 1;
    default:
      return -1;
  }
  return -1;
}


/// Parses a numerical expression from the given context.
///
/// Numerical expressions count as integer and floating point literals.
static std::unique_ptr<Expr> parse_numerical_expr(std::unique_ptr<ASTContext> &ctx) {
  struct Token token = ctx->last();
  ctx->next();  // eat the literal

  if (token.is_int()) {
    return std::make_unique<IntegerLiteral>(std::stoll(token.value), ctx->resolve_type("i64"), token.meta);
  } else if (token.is_float()) {
    return std::make_unique<FPLiteral>(std::stod(token.value), ctx->resolve_type("float"), token.meta);
  }

  return warn_expr("unknown literal kind: " + std::to_string(token.kind), token.meta);
}


/// Parses a single character expression from the given context.
///
/// Character and byte expressions are single character literals.
static std::unique_ptr<Expr> parse_character_expr(std::unique_ptr<ASTContext> &ctx) {
  struct Token token = ctx->last();
  ctx->next();  // eat the character or byte token

  if (token.is_char()) {
    return std::make_unique<CharLiteral>(token.value[0], ctx->resolve_type("char"), token.meta);
  }

  return warn_expr("unknown character or byte kind: " + std::to_string(token.kind), ctx->last().meta);
}


/// Parses a string expression from the given context.
///
/// String and byte string expressions are sequences of characters.
static std::unique_ptr<Expr> parse_string_expr(std::unique_ptr<ASTContext> &ctx) {
  struct Token token = ctx->last();
  ctx->next();  // eat the string or byte string token

  if (token.is_str()) {
    return std::make_unique<StringLiteral>(token.value, ctx->resolve_type("str"), token.meta);
  }

  return warn_expr("unknown string or byte string kind: " + std::to_string(token.kind), ctx->last().meta);
}


/// Parses a boolean expression from the given context.
///
/// Boolean expressions are either `true` or `false` identifiers.
static std::unique_ptr<Expr> parse_boolean_expr(std::unique_ptr<ASTContext> &ctx) {
  struct Token token = ctx->last();
  ctx->next();  // eat the boolean token

  if (token.value != "true" && token.value != "false") {
    return warn_expr("invalid boolean token: " + token.value, token.meta);
  }

  return std::make_unique<BooleanLiteral>(token.value == "true", ctx->resolve_type("bool"), token.meta);
}


/// Parses a function call expression from the given context.
///
/// Function call expressions are in the form `foo(...)`.
static std::unique_ptr<Expr> parse_call_expr(std::unique_ptr<ASTContext> &ctx, const std::string &callee, const Metadata &meta) {
  ctx->next();  // eat the open parenthesis

  std::vector<std::unique_ptr<Expr>> args;
  while (!ctx->last().is_close_paren()) {
    std::unique_ptr<Expr> arg = parse_expr(ctx);
    if (!arg) {
      return warn_expr("expected expression in function call", ctx->last().meta);
    }
    args.push_back(std::move(arg));

    if (ctx->last().is_close_paren()) {
      break;
    }

    if (!ctx->last().is_comma()) {
      return warn_expr("expected ','", ctx->last().meta);
    }

    ctx->next();  // eat comma
  }
  ctx->next();  // eat the close parenthesis

  return std::make_unique<CallExpr>(callee, std::move(args), meta);
}


/// Parses a struct construction expression from the given context.
static std::unique_ptr<Expr> parse_init_expr(std::unique_ptr<ASTContext> &ctx, const std::string &ident, const Metadata &meta) {
  ctx->next();  // eat open brace

  std::vector<std::pair<std::string, std::unique_ptr<Expr>>> fields;
  while (!ctx->last().is_close_brace()) {
    if (!ctx->last().is_ident()) {
      return warn_expr("expected identifier in struct expression", ctx->last().meta);
    }

    const std::string field_name = ctx->last().value;
    ctx->next();  // eat field name

    if (!ctx->last().is_colon()) {
      return warn_expr("expected ':'", ctx->last().meta);
    }
    ctx->next();  // eat colon

    std::unique_ptr<Expr> field_expr = parse_expr(ctx);
    if (!field_expr) {
      return warn_expr("expected expression after ':'", ctx->last().meta);
    }

    // check that the field isnt duplicated
    for (const auto &[name, _] : fields) {
      if (name == field_name) {
        return warn_expr("duplicate field: " + field_name, ctx->last().meta);
      }
    }

    fields.push_back(std::make_pair(field_name, std::move(field_expr)));

    if (ctx->last().is_close_brace()) {
      break;
    }

    if (!ctx->last().is_comma()) {
      return warn_expr("expected ','", ctx->last().meta);
    }

    ctx->next();  // eat comma
  }
  ctx->next();  // eat close brace

  return std::make_unique<InitExpr>(ident, ctx->resolve_type(ident), std::move(fields), meta);
}


/// Parses a struct function call expression from the given context.
static std::unique_ptr<Expr> parse_member_call_expr(std::unique_ptr<ASTContext> &ctx, 
  const std::string &base, const std::string &callee, const Metadata &base_meta, const Metadata &callee_meta) {
  ctx->next();  // eat the dot operator

  std::vector<std::unique_ptr<Expr>> args;
  while (!ctx->last().is_close_paren()) {
    std::unique_ptr<Expr> arg = parse_expr(ctx);
    if (!arg) {
      warn_expr("expected expression in function call", ctx->last().meta);
      return nullptr;
    }
    args.push_back(std::move(arg));

    if (ctx->last().is_close_paren()) {
      break;
    }

    if (!ctx->last().is_comma()) {
      warn_expr("expected ','", ctx->last().meta);
      return nullptr;
    }

    ctx->next();  // eat comma
  }
  ctx->next();  // eat the close parenthesis

  Decl *d = curr_scope->get_decl(base);
  if (!d) {
    return warn_expr("unresolved identifier: " + base, ctx->last().meta);
  }

  VarDecl *str_decl = dynamic_cast<VarDecl *>(d);
  if (!str_decl) {
    return warn_expr("expected struct type", ctx->last().meta);
  }

  return std::make_unique<MemberCallExpr>(
    std::make_unique<DeclRefExpr>(base, str_decl->get_type(), base_meta), callee, std::move(args),
    callee_meta);
}


/// Parses a struct member access expression from the given context.
static std::unique_ptr<Expr> parse_member_expr(std::unique_ptr<ASTContext> &ctx, const std::string &base, const Metadata &meta) {
  ctx->next();  // eat the dot operator

  // verify that the base exists in this scope
  Decl *d = curr_scope->get_decl(base);
  if (!d) {
    return warn_expr("unresolved identifier: " + base, ctx->last().meta);
  }

  // verify that the base is a struct
  VarDecl *str_decl = dynamic_cast<VarDecl *>(d);
  if (!str_decl) {
    return warn_expr("expected struct type", ctx->last().meta);
  }

  if (!ctx->last().is_ident()) {
    return warn_expr("expected identifier after '.'", ctx->last().meta);
  }

  const std::string field = ctx->last().value;
  const Metadata field_meta = ctx->last().meta;
  ctx->next();  // eat field name

  if (ctx->last().is_open_paren()) {
    return parse_member_call_expr(ctx, base, field, meta, field_meta);
  }

  /*
  // verify that the field exists in the struct
  if (!str_decl->has_field(field)) {
    return warn_expr("unresolved field: " + field, ctx->last().meta);
  }
  */

  return std::make_unique<MemberExpr>(
    std::make_unique<DeclRefExpr>(base, str_decl->get_type(), meta), field, field_meta);
}


/// Parses an array access expression from the given context.
///
/// Array access expressions are in the form of `<expr>[<expr>]`.
static std::unique_ptr<Expr> parse_array_access_expr(std::unique_ptr<ASTContext> &ctx, const std::string &base, const Metadata &meta) {
  const Metadata access_meta = ctx->last().meta;
  ctx->next();  // eat open bracket

  std::unique_ptr<Expr> index = parse_expr(ctx);
  if (!index) {
    return warn_expr("expected expression in array access", ctx->last().meta);
  }

  if (!ctx->last().is_close_bracket()) {
    return warn_expr("expected ']'", ctx->last().meta);
  }
  ctx->next();  // eat close bracket

  // verify that the base exists in this scope
  Decl *d = curr_scope->get_decl(base);
  if (!d) {
    return warn_expr("unresolved identifier: " + base, ctx->last().meta);
  }

  // verify that the base is an array
  VarDecl *arr_decl = dynamic_cast<VarDecl *>(d);
  if (!arr_decl) {
    return warn_expr("expected array type", ctx->last().meta);
  }

  return std::make_unique<ArrayAccessExpr>(
    std::make_unique<DeclRefExpr>(base, arr_decl->get_type(), meta), std::move(index), access_meta);
}


/// Parses an identifier expression from the given context.
///
/// Identifiers are used to reference variables, function calls, etc.
static std::unique_ptr<Expr> parse_identifier_expr(std::unique_ptr<ASTContext> &ctx) {
  struct Token token = ctx->last();
  ctx->next();  // eat the identifier

  if (ctx->last().is_open_paren()) {
    return parse_call_expr(ctx, token.value, token.meta);
  } else if (ctx->last().is_dot()) {
    return parse_member_expr(ctx, token.value, token.meta);
  } else if (ctx->last().is_open_bracket()) {
    return parse_array_access_expr(ctx, token.value, token.meta);
  } else if (VarDecl *d = dynamic_cast<VarDecl *>(curr_scope->get_decl(token.value))) {
    return std::make_unique<DeclRefExpr>(token.value, d->get_type(), token.meta);
  } else if (ParamVarDecl *d = dynamic_cast<ParamVarDecl *>(curr_scope->get_decl(token.value))) {
    return std::make_unique<DeclRefExpr>(token.value, d->get_type(), token.meta);
  }

  if (ctx->last().is_open_brace()) {
    return parse_init_expr(ctx, token.value, token.meta);
  }

  if (Decl *d = curr_scope->get_decl(token.value)) {
    if (VarDecl *v = dynamic_cast<VarDecl *>(d)) {
      return std::make_unique<DeclRefExpr>(token.value, v->get_type(), token.meta);
    }
  }
  return std::make_unique<DeclRefExpr>(token.value, nullptr, token.meta);
}


/// Parses a unary expression from the given context.
///
/// Unary expressions are expressions that involve a single operand.
static std::unique_ptr<Expr> parse_unary_expr(std::unique_ptr<ASTContext> &ctx) {
  TokenKind op_kind = ctx->last().kind;
  ctx->next();  // eat operator

  UnaryOp oper = get_unary_op(op_kind);
  if (oper == UnaryOp::UnknownUnaryOp) {
    return warn_expr("unknown unary operator: " + std::to_string(op_kind), ctx->last().meta);
  }

  std::unique_ptr<Expr> base = parse_primary_expr(ctx);
  if (!base) {
    return warn_expr("expected expression after unary operator", ctx->last().meta);
  }

  return std::make_unique<UnaryExpr>(oper, std::move(base), base->get_meta());
}


/// Parses a primary expression from the given context.
///
/// Primary expressions are the most basic form of expressions.
static std::unique_ptr<Expr> parse_primary_expr(std::unique_ptr<ASTContext> &ctx) {
  if (ctx->last().is_int() || ctx->last().is_float()) {
    return parse_numerical_expr(ctx);
  }

  if (ctx->last().is_char()) {
    return parse_character_expr(ctx);
  }

  if (ctx->last().is_str()) {
    return parse_string_expr(ctx);
  }

  if (ctx->last().is_bool()) {
    return parse_boolean_expr(ctx);
  }

  if (ctx->last().is_null()) {
    ctx->next();  // eat the `null` identifier
    return std::make_unique<NullExpr>(nullptr, ctx->last().meta);
  }

  if (ctx->last().is_ident()) {
    return parse_identifier_expr(ctx);
  }

  return parse_unary_expr(ctx);
  //return warn_expr("unknown primary expression kind: " + std::to_string(ctx->last().lit_kind), ctx->last().meta);
}


/// Parses a binary expression from the given context.
///
/// Binary expressions are expressions that involve two operands.
static std::unique_ptr<Expr> parse_binary_expr(std::unique_ptr<ASTContext> &ctx, std::unique_ptr<Expr> base, int precedence) {
  while (1) {
    int token_prec = get_precedence(ctx->last().kind);

    if (token_prec < precedence) {
      return base;
    }

    TokenKind op_kind = ctx->last().kind;
    ctx->next();  // eat operator

    BinaryOp oper = get_binary_op(op_kind);
    if (oper == BinaryOp::UnknownBinaryOp) {
      return warn_expr("unknown binary operator: " + std::to_string(op_kind), ctx->last().meta);
    }

    std::unique_ptr<Expr> rval = parse_primary_expr(ctx);
    if (!rval) {
      return warn_expr("expected expression after binary operator", ctx->last().meta);
    }

    int next_prec = get_precedence(ctx->last().kind);
    if (token_prec < next_prec) {
      rval = parse_binary_expr(ctx, std::move(rval), token_prec + 1);
      if (!rval) {
        return warn_expr("expected expression after binary operator", ctx->last().meta);
      }
    }

    base = std::make_unique<BinaryExpr>(oper, std::move(base), std::move(rval), base->get_meta());
  }
}


/// Parses a generic expression from the given context.
static std::unique_ptr<Expr> parse_expr(std::unique_ptr<ASTContext> &ctx) {
  std::unique_ptr<Expr> base = parse_primary_expr(ctx);
  if (!base) {
    return warn_expr("expected expression", ctx->last().meta);
  }
  return parse_binary_expr(ctx, std::move(base), 0);
}


/// Parses a compound statement from the given context.
///
/// Compound statements are a list of statements with a scope.
static std::unique_ptr<Stmt> parse_compound_stmt(std::unique_ptr<ASTContext> &ctx) {
  const Metadata meta = ctx->last().meta;
  ctx->next();  // eat open brace

  // declare new scope for the block
  std::shared_ptr<Scope> scope = std::make_shared<Scope>(curr_scope, ScopeContext{ .is_compound_scope = true });
  curr_scope = scope;

  std::vector<std::unique_ptr<Stmt>> stmts;
  while (!ctx->last().is_close_brace()) {
    std::unique_ptr<Stmt> stmt = parse_stmt(ctx);
    if (!stmt) {
      return warn_stmt("expected statement in block", ctx->last().meta);
    }
    stmts.push_back(std::move(stmt));

    if (!ctx->last().is_semi() && !ctx->last_two().is_close_brace()) {
      return warn_stmt("expected ';'", ctx->last().meta);
    }

    if (ctx->last().is_semi()) {
      ctx->next();  // eat semi
    }
  }
  ctx->next();  // eat close brace

  // move back up to the parent scope
  curr_scope = curr_scope->get_parent();
  return std::make_unique<CompoundStmt>(std::move(stmts), scope, meta);
}


/// Parses a return statement from the given context.
///
/// Return statements appear in the form `return <expr>`, where <expr> may be implicitly null.
static std::unique_ptr<Stmt> parse_return_stmt(std::unique_ptr<ASTContext> &ctx) {
  const Metadata meta = ctx->last().meta;
  ctx->next();  // eat return keyword

  if (ctx->last().is_semi()) {
    return std::make_unique<ReturnStmt>(std::make_unique<NullExpr>(nullptr, meta), meta);
  }

  std::unique_ptr<Expr> expr = parse_expr(ctx);
  if (!expr) {
    return warn_stmt("expected expression after 'return'", meta);
  }

  return std::make_unique<ReturnStmt>(std::move(expr), meta);
}


/// Parses an if statement from the given context.
///
/// If statements appear in the form `if <expr> { <stmt> } else { <stmt> }`.
static std::unique_ptr<Stmt> parse_if_stmt(std::unique_ptr<ASTContext> &ctx) {
  const Metadata meta = ctx->last().meta;
  ctx->next();  // eat if keyword

  std::unique_ptr<Expr> cond = parse_expr(ctx);
  if (!cond) {
    return warn_stmt("expected expression after 'if'", ctx->last().meta);
  }

  std::unique_ptr<Stmt> then_body = parse_stmt(ctx);
  if (!then_body) {
    return warn_stmt("expected statement after 'if' condition", ctx->last().meta);
  }

  if (ctx->last().is_kw("else")) {
    ctx->next();  // eat else keyword

    std::unique_ptr<Stmt> else_body = parse_stmt(ctx);
    if (!else_body) {
      return warn_stmt("expected statement after 'else'", ctx->last().meta);
    }

    return std::make_unique<IfStmt>(std::move(cond), std::move(then_body), std::move(else_body), meta);
  }

  return std::make_unique<IfStmt>(std::move(cond), std::move(then_body), nullptr, meta);
}


/// Parses an until statement from the given context.
///
/// Until statements appear in the form `until <expr> { <stmt> }`.
static std::unique_ptr<Stmt> parse_until_stmt(std::unique_ptr<ASTContext> &ctx) {
  const Metadata meta = ctx->last().meta;
  ctx->next();  // eat until keyword

  std::unique_ptr<Expr> cond = parse_expr(ctx);
  if (!cond) {
    return warn_stmt("expected expression after 'until'", ctx->last().meta);
  }

  std::unique_ptr<Stmt> body = parse_stmt(ctx);
  if (!body) {
    return warn_stmt("expected statement after 'until' condition", ctx->last().meta);
  }

  return std::make_unique<UntilStmt>(std::move(cond), std::move(body), meta);
}


/// Parses a match statement from the given context.
///
/// Match statements appear in the form `match <expr> { case <expr> => <stmt>, ... }`.
static std::unique_ptr<Stmt> parse_match_stmt(std::unique_ptr<ASTContext> &ctx) {
  const Metadata meta = ctx->last().meta;
  ctx->next();  // eat match keyword

  std::unique_ptr<Expr> match_expr = parse_expr(ctx);
  if (!match_expr) {
    return warn_stmt("expected expression after 'match'", ctx->last().meta);
  }

  if (!ctx->last().is_open_brace()) {
    return warn_stmt("expected '{' after 'match' expression", ctx->last().meta);
  }
  ctx->next();  // eat open brace

  std::vector<std::unique_ptr<MatchCase>> cases;
  while (!ctx->last().is_close_brace()) {
    std::unique_ptr<Expr> case_expr;
    if (ctx->last().is_ident() && ctx->last().value == "_") {
      const Metadata default_meta = ctx->last().meta;
      ctx->next();  // eat default token
      case_expr = std::make_unique<DefaultExpr>(nullptr, default_meta);
    } else {
      case_expr = parse_expr(ctx);
    }

    if (!case_expr) {
      return warn_stmt("expected expression after 'case'", ctx->last().meta);
    }

    if (!ctx->last().is_fat_arrow()) {
      return warn_stmt("expected '=>' after 'case' expression", ctx->last().meta);
    }
    ctx->next();  // eat fat arrow

    std::unique_ptr<Stmt> case_stmt = parse_stmt(ctx);
    if (!case_stmt) {
      return warn_stmt("expected statement after 'case' expression", ctx->last().meta);
    }
    cases.push_back(std::make_unique<MatchCase>(std::move(case_expr), std::move(case_stmt), case_stmt->get_meta()));
    
    if (ctx->last().is_close_brace()) {
      break;
    }

    if (!ctx->last().is_comma()) {
      return warn_stmt("expected ','", ctx->last().meta);
    }
    ctx->next();  // eat comma
  }
  ctx->next();  // eat close brace

  return std::make_unique<MatchStmt>(std::move(match_expr), std::move(cases), meta);
}


/// Parses a generic statement from the given context.
static std::unique_ptr<Stmt> parse_stmt(std::unique_ptr<ASTContext> &ctx) {
  if (ctx->last().is_open_brace()) {
    return parse_compound_stmt(ctx);
  }

  if (!ctx->last().is_ident()) {
    return warn_stmt("unexpected token: " + ctx->last().value, ctx->last().meta);
  }

  if (ctx->last().is_kw("if")) {
    return parse_if_stmt(ctx);
  }

  if (ctx->last().is_kw("let")) {
    return parse_var_decl(ctx);
  }

  if (ctx->last().is_kw("match")) {
    return parse_match_stmt(ctx);
  }

  if (ctx->last().is_kw("return")) {
    return parse_return_stmt(ctx);
  }

  if (ctx->last().is_kw("until")) {
    return parse_until_stmt(ctx);
  }

  return parse_expr(ctx);
}


/// Parses a variable declaration from the given context.
///
/// Variable declarations are in the form of `let 'mut' <identifier> = <expr>;`.
static std::unique_ptr<Stmt> parse_var_decl(std::unique_ptr<ASTContext> &ctx) {
  const Metadata decl_meta = ctx->last().meta;
  ctx->next();  // eat let keyword

  bool is_mutable = false;
  if (ctx->last().is_kw("mut")) {
    is_mutable = true;
    ctx->next();  // eat mut keyword
  }

  if (!ctx->last().is_ident()) {
    return warn_stmt("expected identifier after 'let'", ctx->last().meta);
  }

  const std::string name = ctx->last().value;
  const Metadata meta = ctx->last().meta;
  ctx->next();  // eat variable name

  if (!ctx->last().is_colon()) {
    return warn_stmt("expected ':'", ctx->last().meta);
  }
  ctx->next();  // eat separator

  bool is_rune = false;
  if (ctx->last().is_hash()) {
    is_rune = true;
    ctx->next();  // eat hash
  }

  if (!ctx->last().is_ident()) {
    return warn_stmt("expected type identifier", ctx->last().meta);
  }
  std::string type = ctx->last().value;
  ctx->next();  // eat type

  if (ctx->last().is_semi()) {
    // prevent immutable empty declarations
    if (!is_mutable) {
      return warn_stmt("immutable declaration must be initialized", ctx->last().meta);
    }

    std::unique_ptr<VarDecl> decl = std::make_unique<VarDecl>(name, ctx->resolve_type(type), is_mutable, is_rune, meta);

    // add declaration to parent scope
    curr_scope->add_decl(decl.get());
    return std::make_unique<DeclStmt>(std::move(decl), decl_meta);
  } else if (!ctx->last().is_eq()) {
    return warn_stmt("expected ';'", ctx->last().meta);
  }
  ctx->next();  // eat eq

  std::unique_ptr<Expr> value = parse_expr(ctx);
  if (!value) {
    return warn_stmt("expected expression after '='", ctx->last().meta);
  }

  std::unique_ptr<VarDecl> decl = std::make_unique<VarDecl>(name, ctx->resolve_type(type), std::move(value), is_mutable, is_rune, meta);

  // add declaration to parent scope
  curr_scope->add_decl(decl.get());
  return std::make_unique<DeclStmt>(std::move(decl), decl_meta);
}


/// Parses an enum declaration from the given context.
///
/// Enum declarations are in the form of `enum <identifier> { <variants> }`.
static std::unique_ptr<EnumDecl> parse_enum_decl(std::unique_ptr<ASTContext> &ctx) {
  ctx->next();  // eat enum keyword

  if (!ctx->last().is_ident()) {
    return warn_enum("expected identifier after 'enum'", ctx->last().meta);
  }

  const std::string name = ctx->last().value;
  const Metadata meta = ctx->last().meta;
  ctx->next();  // eat enum name

  if (!ctx->last().is_open_brace()) {
    return warn_enum("expected '{' after enum identifier", ctx->last().meta);
  }
  ctx->next();  // eat open brace

  std::vector<std::unique_ptr<EnumVariantDecl>> variants;
  while (!ctx->last().is_close_brace()) {
    if (!ctx->last().is_ident()) {
      return warn_enum("expected identifier in enum variant list", ctx->last().meta);
    }

    const std::string variant_name = ctx->last().value;
    const Metadata variant_meta = ctx->last().meta;
    ctx->next();  // eat variant name
    
    // verify the variant does not already exist
    for (const std::unique_ptr<EnumVariantDecl> &v : variants) {
      if (v->get_name() == variant_name) {
        return warn_enum("variant already exists: " + variant_name + " in " + name, ctx->last().meta);
      }
    }

    variants.push_back(std::make_unique<EnumVariantDecl>(variant_name, variant_meta));
    if (ctx->last().is_close_brace()) {
      break;
    }
    
    if (!ctx->last().is_comma()) {
      return warn_enum("expected ','", ctx->last().meta);
    }
    ctx->next();  // eat comma
  }
  ctx->next();  // eat close brace

  std::unique_ptr<EnumDecl> enumeration = std::make_unique<EnumDecl>(name, std::move(variants), meta);

  // add declaration to parent scope
  curr_scope->add_decl(enumeration.get());
  return enumeration;
}


/// Parses a function declaration from the given context.
///
/// Function declarations are in the form of `fn <identifier>(<args>) -> <return_ty> { <body> }`.
static std::unique_ptr<FunctionDecl> parse_fn_decl(std::unique_ptr<ASTContext> &ctx) {
  ctx->next();  // eat fn keyword

  if (!ctx->last().is_ident()) {
    return warn_fn("expected identifier after 'fn'", ctx->last().meta);
  }

  const std::string name = ctx->last().value;
  const Metadata meta = ctx->last().meta;
  ctx->next();  // eat function name

  if (!ctx->last().is_open_paren()) {
    return warn_fn("expected '(' after function identifier", ctx->last().meta);
  }
  ctx->next();  // eat open paren

  std::vector<std::unique_ptr<ParamVarDecl>> params;
  while (!ctx->last().is_close_paren()) {
    if (!ctx->last().is_ident()) {
      return warn_fn("expected identifier in function parameter list", ctx->last().meta);
    }

    const std::string param_name = ctx->last().value;
    const Metadata param_meta = ctx->last().meta;
    ctx->next();  // eat param name

    if (!ctx->last().is_colon()) {
      return warn_fn("expected ':' in function parameter list", ctx->last().meta);
    }

    ctx->next();  // eat colon

    if (!ctx->last().is_ident()) {
      return warn_fn("expected type in function parameter list", ctx->last().meta);
    }

    const std::string param_type = ctx->last().value;
    ctx->next();  // eat param type

    std::unique_ptr<ParamVarDecl> param = std::make_unique<ParamVarDecl>(param_name, ctx->resolve_type(param_type), param_meta);

    if (curr_scope->get_decl(param_name)) {
      return warn_fn("parameter identifier already exists in scope: " + param_name, ctx->last().meta);
    }

    // add param to function scope
    curr_scope->add_decl(param.get());
    params.push_back(std::move(param));

    if (ctx->last().is_comma()) {
      ctx->next();  // eat comma
    }
  }
  ctx->next();  // eat close paren

  std::string ret_type = "void";
  if (ctx->last().is_arrow()) {
    ctx->next();  // eat arrow

    if (!ctx->last().is_ident()) {
      return warn_fn("expected return type in function declaration", ctx->last().meta);
    }
    
    ret_type = ctx->last().value;
    ctx->next();  // eat return type
  }

  if (ctx->last().is_semi()) {
    ctx->next();  // eat semi
    return std::make_unique<FunctionDecl>(name, ctx->resolve_type(ret_type), std::move(params), meta);
  }

  if (!ctx->last().is_open_brace() ) {
    return warn_fn("expected '{' or ';' in function declaration", ctx->last().meta);
  }

  // assign new scope for the function body
  std::shared_ptr<Scope> scope = std::make_shared<Scope>(curr_scope, ScopeContext{ .is_func_scope = true });
  curr_scope = scope;

  std::unique_ptr<Stmt> body = parse_stmt(ctx);
  if (!body) {
    return warn_fn("expected function body", ctx->last().meta);
  }

  std::unique_ptr<FunctionDecl> function = std::make_unique<FunctionDecl>(
    name, ctx->resolve_type(ret_type), std::move(params), std::move(body), scope, meta);

  // move back to parent scope
  curr_scope = curr_scope->get_parent();
  
  // add function declaration to parent scope
  curr_scope->add_decl(function.get());
  return function;
}


/// Parses a struct declaration from the given context.
///
/// Struct declarations are in the form of `struct <identifier> { <fields> }`.
static std::unique_ptr<TypeDecl> parse_struct_decl(std::unique_ptr<ASTContext> &ctx) {
  ctx->next();  // eat struct keyword

  if (!ctx->last().is_ident()) {
    return warn_tydecl("expected identifier after 'struct'", ctx->last().meta);
  }

  const std::string name = ctx->last().value;
  const Metadata meta = ctx->last().meta;
  ctx->next();  // eat struct name

  if (!ctx->last().is_open_brace()) {
    return warn_tydecl("expected '{' after struct identifier", ctx->last().meta);
  }
  ctx->next();  // eat open brace

  // declare new scope for the struct block
  std::shared_ptr<Scope> scope = std::make_shared<Scope>(curr_scope, ScopeContext{ .is_struct_scope = true });
  curr_scope = scope;

  // parse fields and methods
  std::vector<std::unique_ptr<FieldDecl>> fields;
  while (!ctx->last().is_close_brace()) {
    if (!ctx->last().is_ident()) {
      return warn_tydecl("expected identifier", ctx->last().meta);
    }

    bool is_private = false;
    if (ctx->last().is_kw("priv")) {
      is_private = true;
      ctx->next();  // eat priv keyword
    }

    const std::string field_name = ctx->last().value;
    const Metadata field_meta = ctx->last().meta;
    ctx->next();  // eat field name

    if (!ctx->last().is_colon()) {
      return warn_tydecl("expected ':'", ctx->last().meta);
    }
    ctx->next();  // eat colon

    if (!ctx->last().is_ident()) {
      return warn_tydecl("expected type", ctx->last().meta);
    }

    const std::string field_type = ctx->last().value;
    ctx->next();  // eat field type

    // verify that the field does not already exist
    for (const std::unique_ptr<FieldDecl> &f : fields) {
      if (f->get_name() == field_name) {
        return warn_tydecl("field already exists: " + field_name + " in " + name, ctx->last().meta);
      }
    }

    std::unique_ptr<FieldDecl> field = std::make_unique<FieldDecl>(
      field_name, ctx->resolve_type(field_type), field_meta);
    if (is_private) {
      field->set_priv();
    }

    // add field to struct scope
    curr_scope->add_decl(field.get());
    fields.push_back(std::move(field));

    if (!ctx->last().is_comma()) {
      return warn_tydecl("expected ','", ctx->last().meta);
    }
    ctx->next();  // eat comma
  }
  ctx->next();  // eat close brace

  std::unique_ptr<StructDecl> structure = std::make_unique<StructDecl>(name, std::move(fields), scope, meta);
  structure->set_type(new StructType(structure->get_name()));
  // move back to parent scope
  curr_scope = curr_scope->get_parent();

  // add struct declaration to parent scope
  curr_scope->add_decl(static_cast<TypeDecl *>(structure.get()));
  return structure;
}


/// Parses a trait declaration from the given context.
///
/// Trait declarations are in the form of `trait <identifier> { <methods> }`.
static std::unique_ptr<TraitDecl> parse_trait_decl(std::unique_ptr<ASTContext> &ctx) {
  ctx->next();  // eat trait keyword

  if (!ctx->last().is_ident()) {
    return warn_trait("expected identifier after 'trait'", ctx->last().meta);
  }

  const std::string name = ctx->last().value;
  const Metadata meta = ctx->last().meta;
  ctx->next();  // eat trait name

  if (!ctx->last().is_open_brace()) {
    return warn_trait("expected '{' after trait identifier", ctx->last().meta);
  }
  ctx->next();  // eat open brace

  std::vector<std::unique_ptr<FunctionDecl>> methods;
  while (!ctx->last().is_close_brace()) {
    if (ctx->last().is_kw("priv")) {
      return warn_trait("method cannot be declared private in trait '" + name + "'", ctx->last().meta);
    }

    std::unique_ptr<FunctionDecl> method = parse_fn_decl(ctx);
    if (!method) {
      return warn_trait("expected method in trait declaration", ctx->last().meta);
    } 

    if (method->has_body()) {
      return warn_trait("method '" + method->get_name() + "' cannot have a body in trait declaration", ctx->last().meta);
    }

    methods.push_back(std::move(method));
  }
  ctx->next();  // eat close brace

  std::unique_ptr<TraitDecl> trait = std::make_unique<TraitDecl>(name, std::move(methods), meta);

  // add trait declaration to parent scope
  curr_scope->add_decl(trait.get());
  return trait;
}


/// Parses an impl declaration from the given context.
///
/// Impl declarations are in the form of `impl <struct> { <methods> }` or `impl <trait> for <struct> { <methods> }`.
static std::unique_ptr<ImplDecl> parse_impl_decl(std::unique_ptr<ASTContext> &ctx) {
  ctx->next();  // eat impl keyword

  if (!ctx->last().is_ident()) {
    return warn_impl("expected identifier after 'impl'", ctx->last().meta);
  }

  std::string target = ctx->last().value;
  std::string trait = "";
  const Metadata meta = ctx->last().meta;
  ctx->next();  // eat first name
  if (ctx->last().is_kw("for")) {
    ctx->next();  // eat for keyword

    if (!ctx->last().is_ident()) {
      return warn_impl("expected identifier after 'for'", ctx->last().meta);
    }

    trait = target;
    target = ctx->last().value;
    ctx->next();  // eat second name
  }

  if (!ctx->last().is_open_brace()) {
    return warn_impl("expected '{'", ctx->last().meta);
  }
  ctx->next();  // eat open brace

  std::vector<std::unique_ptr<FunctionDecl>> methods;
  while (!ctx->last().is_close_brace()) {
    bool is_private = false;
    if (ctx->last().is_kw("priv")) {
      is_private = true;
      ctx->next();  // eat priv keyword
    }

    std::unique_ptr<FunctionDecl> method = parse_fn_decl(ctx);
    if (!method) {
      return warn_impl("expected method in impl declaration", ctx->last().meta);
    }
    // check that method was not already implemented
    for (const std::unique_ptr<FunctionDecl> &m : methods) {
      if (m->get_name() == method->get_name()) {
        return warn_impl("method already exists: " + method->get_name() + " in " + target, ctx->last().meta);
      }
    }

    if (is_private) {
      method->set_priv();
    }
    methods.push_back(std::move(method));
  }
  ctx->next();  // eat close brace
  return std::make_unique<ImplDecl>(trait, target, std::move(methods), meta);
}


/// Parses a declaration from the given context.
///
/// Declarations are the top-level constructs in a package.
static std::unique_ptr<Decl> parse_decl(std::unique_ptr<ASTContext> &ctx, bool is_private) {
  if (ctx->last().is_kw("fn")) {
    if (is_private) {
      if (std::unique_ptr<FunctionDecl> decl = parse_fn_decl(ctx)) {
        decl->set_priv();
        return decl;
      }
      return warn_decl("expected function declaration", ctx->last().meta);
    }
    return parse_fn_decl(ctx);
  }

  if (ctx->last().is_kw("struct")) {
    if (is_private) {
      if (std::unique_ptr<TypeDecl> decl = parse_struct_decl(ctx)) {
        decl->set_priv();
        return decl;
      }
      return warn_decl("expected struct declaration", ctx->last().meta);
    }
    return parse_struct_decl(ctx);
  }

  if (ctx->last().is_kw("trait")) {
    if (is_private) {
      if (std::unique_ptr<TraitDecl> decl = parse_trait_decl(ctx)) {
        decl->set_priv();
        return decl;
      }
      return warn_decl("expected trait declaration", ctx->last().meta);
    }
    return parse_trait_decl(ctx);
  }

  if (ctx->last().is_kw("impl")) {
    if (is_private) {
      return warn_decl("impl cannot be declared private", ctx->last().meta);
    }
    return parse_impl_decl(ctx);
  }

  if (ctx->last().is_kw("enum")) {
    if (is_private) {
      if (std::unique_ptr<EnumDecl> decl = parse_enum_decl(ctx)) {
        decl->set_priv();
        return decl;
      }
      return warn_decl("expected enum declaration", ctx->last().meta);
    }
    return parse_enum_decl(ctx);
  }

  return warn_decl("unknown identifier: " + ctx->last().value, ctx->last().meta);
}


/// Parses a package from the given context.
///
/// A package is a collection of definitions and imports.
static std::unique_ptr<PackageUnit> parse_pkg(std::unique_ptr<ASTContext> &ctx) {
  const std::string name = remove_extension(ctx->file());
  std::vector<std::string> imports;
  std::vector<std::unique_ptr<Decl>> decls;

  // assign this package as parent scope moving forward
  std::shared_ptr<Scope> scope = std::make_shared<Scope>(nullptr, ScopeContext{ .is_pkg_scope = true });
  curr_scope = scope;

  while (!ctx->last().is_eof()) {
    if (ctx->last().is_kw("pkg")) {
      ctx->next();  // eat package keyword

      if (ctx->last().is_ident()) {
        const std::string first = ctx->last().value;
        ctx->next();  // eat first identifier

        if (ctx->last().is_path()) {
          ctx->next();  // eat path operator
          if (!ctx->last().is_ident()) {
            return warn_pkg("expected identifier after '::'", ctx->last().meta);
          }
          imports.push_back(first + '/' + ctx->last().value);
          ctx->next();  // eat name
        } else {
          imports.push_back(first);
        }
        
        if (!ctx->last().is_semi()) {
          return warn_pkg("expected ';'", ctx->last().meta);
        }
        ctx->next();  // eat semi
      } else {
        panic("expected identifier after 'pkg'");
      }

      continue;
    }

    bool is_private = false;
    if (ctx->last().is_kw("priv")) {
      is_private = true;
      ctx->next();  // eat priv keyword
    }

    std::unique_ptr<Decl> decl = parse_decl(ctx, is_private);
    if (!decl) {
      panic("expected declaration or import", ctx->last().meta);
    }

    // add declaration to package scope
    curr_scope->add_decl(decl.get());
    decls.push_back(std::move(decl));
  }

  // clear scope
  curr_scope = nullptr;

  return std::make_unique<PackageUnit>(name, imports, std::move(decls), scope);
}


/// Parses a crate from the given context.
///
/// A crate is a collection of packages and represents a whole program.
static std::unique_ptr<CrateUnit> parse_crate(std::unique_ptr<ASTContext> &ctx) {
  std::vector<std::unique_ptr<PackageUnit>> packages;

  do {
    ctx->next_file();
    if (ctx->last().is_eof()) {
      break;
    }

    std::unique_ptr<PackageUnit> pkg = parse_pkg(ctx);
    if (!pkg) {
      panic("expected package", ctx->last().meta);
    }
    packages.push_back(std::move(pkg));
  } while (!ctx->last().is_eof());

  return std::make_unique<CrateUnit>(std::move(packages));
}


/// Builds an abstract syntax tree from the given context.
std::unique_ptr<CrateUnit> build_ast(std::unique_ptr<ASTContext> &Cctx) {
  return parse_crate(Cctx);
}
