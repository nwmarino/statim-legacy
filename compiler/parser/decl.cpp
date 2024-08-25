/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <memory>
#include <utility>

#include "parser.h"
#include "../core/ast.h"
#include "../core/cctx.h"
#include "../core/logger.h"
#include "../core/symbol.h"

/// Parses a function definition.
///
/// Function definitions are in the form of `fn <identifier>(<args>) -> <return_ty> { <body> }`.
/// The function body is a forced compound statement.
std::unique_ptr<FunctionAST> parse_definition(std::shared_ptr<cctx> ctx) {
  ctx->tk_next(); // eat the `fn` identifier

  ctx->assert_ident();
  std::unique_ptr<PrototypeAST> head = parse_prototype(ctx);
  if (!head) {
    return warn_func("function definition missing prototype", ctx->prev().meta);
  }

  if (std::unique_ptr<Statement> body = parse_stmt(ctx)) {
    // add function to symbol table
    ctx->symb_add(head->get_name(), Symbol(SymbolType::Function));

    return std::make_unique<FunctionAST>(std::move(head), std::move(body));
  }

  return warn_func("function " + head->get_name() + " missing body", ctx->prev().meta);
}


/// Parses a function prototype.
///
/// Function prototypes are a contract in the form of `<identifier>(<args>) -> <return_ty>`.
std::unique_ptr<PrototypeAST> parse_prototype(std::shared_ptr<cctx> ctx) {
  const std::string name = ctx->prev().value;
  ctx->tk_next(); // eat name identifier

  ctx->tk_expect(TokenKind::OpenParen, "'('");

  // parse function arguments
  std::vector<std::pair<std::string, std::string>> args;
  while (ctx->prev().kind == TokenKind::Identifier) {
    const std::string arg_name = ctx->prev().value;
    ctx->tk_next(); // eat the argument name

    ctx->tk_expect(TokenKind::Colon, "':'");

    ctx->assert_ident();
    const std::string arg_type = ctx->prev().value;
    ctx->tk_next(); // eat the type identifier

    args.push_back(std::make_pair(arg_name, arg_type));

    if (ctx->prev().kind == TokenKind::Comma) {
      ctx->tk_next();
    }
  }

  ctx->tk_expect(TokenKind::CloseParen, "')'");

  if (ctx->prev().kind != TokenKind::Arrow) {
    if (ctx->prev().kind == TokenKind::OpenBrace) {
      // implicit void return if no type is provided
      return std::make_unique<PrototypeAST>(name, std::move(args), "void");
    }
    tokexp_panic("'->'", ctx->prev().meta);
  }
  ctx->tk_next(); // eat the arrow

  ctx->assert_ident();
  const std::string ret_type = ctx->prev().value;
  ctx->tk_next(); // eat type identifier

  return std::make_unique<PrototypeAST>(name, std::move(args), ret_type);
}


/// Parses a struct definition.
///
/// Struct definitions are in the form of `struct <identifier> { <fields> }`.
/// Fields are in the form of `<identifier>: <type>`.
std::unique_ptr<StructAST> parse_struct(std::shared_ptr<cctx> ctx) {
  ctx->tk_next(); // eat the `struct` identifier

  ctx->assert_ident();
  const std::string name = ctx->prev().value;
  const struct Metadata meta = ctx->prev().meta;
  ctx->tk_next(); // eat name identifier

  ctx->tk_expect(TokenKind::OpenBrace, "'{'");

  std::vector<std::pair<std::string, std::string>> fields;
  while (ctx->prev().kind == TokenKind::Identifier) {
    const std::string field_name = ctx->prev().value;

    // check that the field does not already exist
    for (const std::pair<std::string, std::string> &field : fields) {
      if (field.first == field_name) {
        struct_field_panic(field_name, name, ctx->prev().meta);
      }
    }
    ctx->tk_next(); // eat field name identifier

    ctx->tk_expect(TokenKind::Colon, "':'");

    ctx->assert_ident();
    const std::string field_type = ctx->prev().value;
    ctx->tk_next(); // eat field type identifier

    fields.push_back(std::make_pair(field_name, field_type));

    if (ctx->prev().kind == TokenKind::CloseBrace) {
      break;
    }

    ctx->tk_expect(TokenKind::Comma, "','");
  }
  ctx->tk_expect(TokenKind::CloseBrace, "'}'");
  return std::make_unique<StructAST>(name, std::move(fields));
}


/// Parses an abstract interface.
///
/// Abstracts are in the form of `abstract <identifier> { <methods> }`.
/// Methods are in the form of `fn <identifier>(<args>) -> <return_ty>;`.
std::unique_ptr<AbstractAST> parse_abstract(std::shared_ptr<cctx> ctx) {
  ctx->tk_next(); // eat the `abstract` identifier

  ctx->assert_ident();
  const std::string name = ctx->prev().value;
  const struct Metadata meta = ctx->prev().meta;
  ctx->tk_next(); // eat the name identifier

  ctx->tk_expect(TokenKind::CloseBrace, "'}'");

  std::vector<std::unique_ptr<PrototypeAST>> methods;
  while (ctx->prev().kind != TokenKind::CloseBrace) {
    if (ctx->symb_is_kw(ctx->prev().value, KeywordType::Fn)) {
      ctx->tk_next(); // eat the `fn` identifier

      if (std::unique_ptr<PrototypeAST> method = parse_prototype(ctx)) {
        // check that the method does not already exist in the abstract
        for (const std::unique_ptr<PrototypeAST> &decl : methods) {
          if (decl->get_name() == method->get_name()) {
            abstract_proto_panic(method->get_name(), name, ctx->prev().meta);
          }
        }
        methods.push_back(std::move(method));
      }
    }

    if (ctx->prev().kind == TokenKind::CloseBrace) {
      break;
    }

    ctx->tk_expect(TokenKind::Semi, "';'");
  }
  ctx->tk_next(); // eat the open brace
  return std::make_unique<AbstractAST>(name, std::move(methods));
}


/// Parses an implementation of an abstract interface.
///
/// Implementations are in the form of `impl <abstract> for <struct> { <methods> }`.
std::unique_ptr<ImplAST> parse_impl(std::shared_ptr<cctx> ctx) {
  ctx->tk_next(); // eat the `impl` identifier

  ctx->assert_ident();
  const std::string abs_name = ctx->prev().value;
  ctx->tk_next(); // eat the abstract name identifier

  if (!ctx->symb_is_kw(ctx->prev().value, KeywordType::For)) {
    tokexp_panic("'for'", ctx->prev().meta);
  }
  ctx->tk_next(); // eat the `for` identifier

  ctx->assert_ident();
  const std::string struct_name = ctx->prev().value;
  ctx->tk_next(); // eat the struct name identifier

  ctx->tk_expect(TokenKind::OpenBrace, "'{'");

  std::vector<std::unique_ptr<FunctionAST>> methods;
  while (ctx->prev().kind != TokenKind::CloseBrace) {
    if (ctx->symb_is_kw(ctx->prev().value, KeywordType::Fn)) {
      if (std::unique_ptr<FunctionAST> method = parse_definition(ctx)) {
        methods.push_back(std::move(method));
      }
    } else {
      impl_panic(ctx->prev().meta);
    }
  }
  ctx->tk_next(); // eat close brace
  return std::make_unique<ImplAST>(abs_name, struct_name, std::move(methods));
}


/// Parses an enumeration definition.
///
/// Enumerations are in the form of `enum <identifier> { <variants> }`.
/// Variants are in the form of `<identifier>, <identifier>, <identifier>, ...`.
std::unique_ptr<EnumAST> parse_enum(std::shared_ptr<cctx> ctx) {
  ctx->tk_next(); // eat the `enum` identifier

  ctx->assert_ident();
  const std::string name = ctx->prev().value;
  const struct Metadata meta = ctx->prev().meta;

  ctx->tk_next(); // eat the name identifier
  ctx->tk_expect(TokenKind::OpenBrace, "'{'");

  std::vector<std::string> variants;
  while (ctx->prev().kind != TokenKind::CloseBrace) {
    ctx->assert_ident();

    // check that the variant does not already exist
    for (const std::string &variant : variants) {
      if (variant == ctx->prev().value) {
        enum_variant_panic(ctx->prev().value, name, ctx->prev().meta);
      }
    }

    variants.push_back(ctx->prev().value);
    ctx->tk_next(); // eat the variant identifier

    if (ctx->prev().kind == TokenKind::CloseBrace) {
      break;
    }

    ctx->tk_expect(TokenKind::Comma, "','");
  }
  ctx->tk_next(); // eat close brace
  return std::make_unique<EnumAST>(name, std::move(variants));
}


/// Parses a variable declaration.
///
/// At this point, the declaration can be mutable or not.
std::unique_ptr<Statement> parse_var_decl(std::shared_ptr<cctx> ctx) {
  switch (ctx->symb_get(ctx->prev().value).keyword)
  {
    case KeywordType::Fix:
      return parse_immut_decl(ctx);

    case KeywordType::Let:
      return parse_mut_decl(ctx);

    default:
      return warn_stmt("expected variable declaration", ctx->prev().meta);
  }
}


/// Parses an immutable variable declaration.
///
/// Immutable declarations are in the form of `fix <identifier>: <type> = <expr>`.
std::unique_ptr<Statement> parse_immut_decl(std::shared_ptr<cctx> ctx) {
  ctx->tk_next(); // eat the `fix` identifier

  ctx->assert_ident();
  const std::string name = ctx->prev().value;
  struct Metadata meta = ctx->prev().meta;
  ctx->tk_next(); // eat the name identifier

  ctx->tk_expect(TokenKind::Colon, "':'");

  ctx->assert_ident();
  const std::string type = ctx->prev().value;
  ctx->tk_next(); // eat the type identifier

  ctx->tk_expect(TokenKind::Eq, "'='");

  if (std::unique_ptr<Expr> expr = parse_expr(ctx)) {
    return std::make_unique<AssignmentStatement>(name, type, std::move(expr));
  }

  return warn_const("expected expression after immutable declaration: " + name, ctx->prev().meta);
}


/// Parses a mutable variable declaration.
///
/// Mutable declarations are in the form of `let <identifier>: <type> = <expr>` or `let <identifier>: <type>`.
std::unique_ptr<Statement> parse_mut_decl(std::shared_ptr<cctx> ctx) {
  ctx->tk_next(); // eat the `let` identifier

  ctx->assert_ident();
  const std::string name = ctx->prev().value;
  struct Metadata meta = ctx->prev().meta;
  ctx->tk_next(); // eat the name identifier

  ctx->tk_expect(TokenKind::Colon, "':'");

  ctx->assert_ident();
  const std::string type = ctx->prev().value;
  ctx->tk_next(); // eat the type identifier

  // if the declaration is only an initialization
  if (ctx->prev().kind == TokenKind::Semi) {
    return std::make_unique<AssignmentStatement>(name, type, std::make_unique<NullExpr>());
  }

  ctx->tk_expect(TokenKind::Eq, "'='");

  if (std::unique_ptr<Expr> expr = parse_expr(ctx)) {
    return std::make_unique<AssignmentStatement>(name, type, std::move(expr));
  }

  return warn_stmt("expected expression after variable declaration: " + name, ctx->prev().meta);
}
