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

  if (ctx->prev().kind != TokenKind::Identifier) {
    tokexp_panic("identifier", ctx->prev().meta);
  }

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

  if (ctx->prev().kind != TokenKind::OpenParen) {
    tokexp_panic("'('", ctx->prev().meta);
  }
  ctx->tk_next(); // eat open parentheses

  // parse function arguments
  std::vector<std::pair<std::string, std::string>> args;
  while (ctx->prev().kind == TokenKind::Identifier) {
    std::string argName = ctx->prev().value;
    ctx->tk_next(); // eat the argument name

    if (ctx->prev().kind != TokenKind::Colon) {
      tokexp_panic("':'", ctx->prev().meta);
    }
    ctx->tk_next(); // eat the separator

    if (ctx->prev().kind != TokenKind::Identifier) {
      tokexp_panic("identifier", ctx->prev().meta);
    }
    std::string arg_type = ctx->prev().value;
    ctx->tk_next(); // eat the type identifier

    args.push_back(std::make_pair(argName, arg_type));

    if (ctx->prev().kind == TokenKind::Comma) {
      ctx->tk_next();
    }
  }

  if (ctx->prev().kind != TokenKind::CloseParen) {
    tokexp_panic("')'", ctx->prev().meta);
  }
  ctx->tk_next(); // eat close parentheses

  if (name == "main") {
    if (args.size() != 0) {
      return warn_proto("entry function 'main' should not possess arguments", ctx->prev().meta);
    }

    return std::make_unique<PrototypeAST>(name, std::move(args), "void");
  }

  if (ctx->prev().kind != TokenKind::Arrow) {
    tokexp_panic("'->'", ctx->prev().meta);
  }
  ctx->tk_next(); // eat the arrow

  if (ctx->prev().kind != TokenKind::Identifier) {
    tokexp_panic("identifier", ctx->prev().meta);
  }

  std::string ret_type = ctx->prev().value;
  ctx->tk_next(); // eat type identifier

  return std::make_unique<PrototypeAST>(name, std::move(args), ret_type);
}


/// Parses a struct definition.
///
/// Struct definitions are in the form of `struct <identifier> { <fields> }`.
/// Fields are in the form of `<identifier>: <type>`.
std::unique_ptr<StructAST> parse_struct(std::shared_ptr<cctx> ctx) {
  ctx->tk_next(); // eat the `struct` identifier

  if (ctx->prev().kind != TokenKind::Identifier) {
    tokexp_panic("identifier", ctx->prev().meta);
  }

  const std::string name = ctx->prev().value;
  ctx->tk_next(); // eat name identifier

  if (ctx->prev().kind != TokenKind::OpenBrace) {
    tokexp_panic("'{'", ctx->prev().meta);
  }
  ctx->tk_next(); // eat open brace

  std::vector<std::pair<std::string, std::string>> fields;
  while (ctx->prev().kind == TokenKind::Identifier) {
    std::string field_name = ctx->prev().value;

    // check that the field does not already exist
    for (const std::pair<std::string, std::string> &field : fields) {
      if (field.first == field_name) {
        struct_field_panic(field_name, name, ctx->prev().meta);
      }
    }
    ctx->tk_next(); // eat field name identifier

    if (ctx->prev().kind != TokenKind::Colon) {
      tokexp_panic("':'", ctx->prev().meta);
    }
    ctx->tk_next(); // eat separator

    if (ctx->prev().kind != TokenKind::Identifier) {
      tokexp_panic("identifier", ctx->prev().meta);
    }

    std::string field_type = ctx->prev().value;
    ctx->tk_next(); // eat field type identifier

    fields.push_back(std::make_pair(field_name, field_type));

    if (ctx->prev().kind == TokenKind::CloseBrace) {
      break;
    }

    if (ctx->prev().kind != TokenKind::Comma) {
      tokexp_panic("','", ctx->prev().meta);
    }
    ctx->tk_next(); // eat the comma
  }

  if (ctx->prev().kind != TokenKind::CloseBrace) {
    tokexp_panic("'}'", ctx->prev().meta);
  }
  ctx->tk_next(); // eat close brace

  // add struct to symbol table
  ctx->symb_add(name, Symbol(SymbolType::Ty));
  return std::make_unique<StructAST>(name, std::move(fields));
}


/// Parses an abstract interface.
///
/// Abstracts are in the form of `abstract <identifier> { <methods> }`.
/// Methods are in the form of `fn <identifier>(<args>) -> <return_ty>;`.
std::unique_ptr<AbstractAST> parse_abstract(std::shared_ptr<cctx> ctx) {
  ctx->tk_next(); // eat the `abstract` identifier

  if (ctx->prev().kind != TokenKind::Identifier) {
    tokexp_panic("identifier", ctx->prev().meta);
  }
  std::string name = ctx->prev().value;
  ctx->tk_next(); // eat the name identifier

  if (ctx->prev().kind != TokenKind::OpenBrace) {
    tokexp_panic("'{'", ctx->prev().meta);
  }
  ctx->tk_next(); // eat open brace

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

    if (ctx->prev().kind != TokenKind::Semi) {
      tokexp_panic("';'", ctx->prev().meta);
    }
    ctx->tk_next(); // eat the semi
  }

  if (ctx->prev().kind != TokenKind::CloseBrace) {
    tokexp_panic("'}'", ctx->prev().meta);
  }
  ctx->tk_next(); // eat close brace

  // add abstract to symbol table
  ctx->symb_add(name, Symbol(SymbolType::Interface));
  return std::make_unique<AbstractAST>(name, std::move(methods));
}


/// Parses an implementation of an abstract interface.
///
/// Implementations are in the form of `impl <abstract> for <struct> { <methods> }`.
std::unique_ptr<ImplAST> parse_impl(std::shared_ptr<cctx> ctx) {
  ctx->tk_next(); // eat the `impl` identifier

  if (ctx->prev().kind != TokenKind::Identifier) {
    tokexp_panic("identifier", ctx->prev().meta);
  }

  std::string abs_name = ctx->prev().value;
  ctx->tk_next(); // eat the abstract name identifier

  if (!ctx->symb_is_kw(ctx->prev().value, KeywordType::For)) {
    tokexp_panic("'for'", ctx->prev().meta);
  }
  ctx->tk_next(); // eat the `for` identifier

  if (ctx->prev().kind != TokenKind::Identifier) {
    tokexp_panic("identifier", ctx->prev().meta);
  }

  std::string struct_name = ctx->prev().value;
  ctx->tk_next(); // eat the struct name identifier

  if (ctx->prev().kind != TokenKind::OpenBrace) {
    tokexp_panic("'{'", ctx->prev().meta);
  }
  ctx->tk_next(); // eat open brace

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

  if (ctx->prev().kind != TokenKind::Identifier) {
    tokexp_panic("identifier", ctx->prev().meta);
  }

  std::string name = ctx->prev().value;
  ctx->tk_next(); // eat the name identifier

  if (ctx->prev().kind != TokenKind::OpenBrace) {
    tokexp_panic("'{'", ctx->prev().meta);
  }
  ctx->tk_next(); // eat open brace

  std::vector<std::string> variants;
  while (ctx->prev().kind != TokenKind::CloseBrace) {
    if (ctx->prev().kind != TokenKind::Identifier) {
      tokexp_panic("identifier", ctx->prev().meta);
    }

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

    if (ctx->prev().kind != TokenKind::Comma) {
      tokexp_panic("','", ctx->prev().meta);
    }
    ctx->tk_next(); // eat the comma
  }
  ctx->tk_next(); // eat close brace

  // add enum to symbol table
  ctx->symb_add(name, Symbol(SymbolType::Ty));
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

  if (ctx->prev().kind != TokenKind::Identifier) {
    tokexp_panic("identifier", ctx->prev().meta);
  }

  const std::string name = ctx->prev().value;
  struct Metadata meta = ctx->prev().meta;

  ctx->tk_next(); // eat the name identifier

  if (ctx->prev().kind != TokenKind::Colon) {
    tokexp_panic("':'", ctx->prev().meta);
  }
  ctx->tk_next(); // eat the separator 

  if (ctx->prev().kind != TokenKind::Identifier) {
    tokexp_panic("identifier", ctx->prev().meta);
  }

  const std::string type = ctx->prev().value;
  ctx->tk_next(); // eat the type identifier

  if (ctx->prev().kind != TokenKind::Eq) {
    tokexp_panic("'='", ctx->prev().meta);
  }
  ctx->tk_next(); // eat the assignment operator

  if (std::unique_ptr<Expr> expr = parse_expr(ctx)) {
    // add constant to the symbol table
    ctx->symb_add(name, Symbol(SymbolType::Constant, meta));
    return std::make_unique<AssignmentStatement>(name, type, std::move(expr));
  }

  return warn_const("expected expression after immutable declaration: " + name, ctx->prev().meta);
}


/// Parses a mutable variable declaration.
///
/// Mutable declarations are in the form of `let <identifier>: <type> = <expr>` or `let <identifier>: <type>`.
std::unique_ptr<Statement> parse_mut_decl(std::shared_ptr<cctx> ctx) {
  ctx->tk_next(); // eat the `let` identifier

  if (ctx->prev().kind != TokenKind::Identifier) {
    tokexp_panic("identifier", ctx->prev().meta);
  }

  const std::string name = ctx->prev().value;
  struct Metadata meta = ctx->prev().meta;
  
  ctx->tk_next(); // eat the name identifier

  if (ctx->prev().kind != TokenKind::Colon) {
    tokexp_panic("':'", ctx->prev().meta);
  }
  ctx->tk_next(); // eat the separator

  if (ctx->prev().kind != TokenKind::Identifier) {
    tokexp_panic("identifier", ctx->prev().meta);
  }

  const std::string type = ctx->prev().value;
  ctx->tk_next(); // eat the type identifier

  // if the declaration is only an initialization
  if (ctx->prev().kind == TokenKind::Semi) {
    // add variable to the symbol table
    ctx->symb_add(name, Symbol(SymbolType::Variable, meta));
    return std::make_unique<AssignmentStatement>(name, type, std::make_unique<NullExpr>());
  }

  if (ctx->prev().kind != TokenKind::Eq) {
    tokexp_panic("'='", ctx->prev().meta);
  }
  ctx->tk_next(); // eat the assignment operator

  if (std::unique_ptr<Expr> expr = parse_expr(ctx)) {
    // add variable to the symbol table
    ctx->symb_add(name, Symbol(SymbolType::Variable, meta));
    return std::make_unique<AssignmentStatement>(name, type, std::move(expr));
  }

  return warn_stmt("expected expression after variable declaration: " + name, ctx->prev().meta);
}
