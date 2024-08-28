/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <iostream>
#include <memory>
#include <utility>

#include "../core/ast.h"
#include "../core/cctx.h"
#include "../core/logger.h"

static std::vector<std::string> pkg_names = {};
static std::shared_ptr<cctx> ctx;

void InitializePass(std::shared_ptr<cctx> cctx) {
  ctx = cctx;
}

void ProgAST::pass() {
  // verify no duplicate packages exist
  for (const std::unique_ptr<PackageAST> &pkg : pkgs) {
    for (const std::unique_ptr<PackageAST> &other : pkgs) {
      if (pkg != other && pkg->to_str(0) == other->to_str(0)) {
        panic("duplicate package", pkg->to_str(0).c_str());
      }
    }
  }

  // add packages to global symbol table
  for (const std::unique_ptr<PackageAST> &pkg : pkgs) {
    ctx->add_symb(pkg->get_name(), Symbol(SymbolKind::Package));
  }

  // pass all packages
  for (std::unique_ptr<PackageAST> &pkg : pkgs) {
    pkg->pass();
  }
}


void PackageAST::pass() {
  // ensure all imported packages exist
  for (const std::string &imp : imports) {
    if (!ctx->get_symb(imp)) {
      panic("package not found", imp.c_str());
    }
  }

  // check that no duplicate imports exist
  for (const std::string &imp : imports) {
    for (const std::string &other : imports) {
      if (imp != other && imp == other) {
        panic("duplicate import", imp.c_str());
      }
    }
  }

  // pass all definitions
  for (std::unique_ptr<AST> &def : defs) {
    def->pass();
  }
}


void PrototypeAST::pass() {
  // check main has no args and returns void
  if (name == "main") {
    if (args.size() != 0) {
      panic("entry function 'main' cannot have arguments", "");
    }
    if (ret_ty != "void") {
      panic("entry function 'main' must return void", "");
    }
  }

  // check for duplicate parameter
  for (const std::pair<std::string, std::string> &arg : args) {
    for (const std::pair<std::string, std::string> &other : args) {
      if (arg != other && arg.first == other.first) {
        panic("duplicate parameter", arg.first.c_str());
      }
    }
  }

  // add function to package symbol table
  ctx->add_symb(name, Symbol(SymbolKind::Function, ctx->prev().meta));
}


void FunctionAST::pass() {
  head->pass();
  body->pass();
}


void CompoundStatement::pass() {
  ctx->decl_scope();

  for (std::unique_ptr<Statement> &stmt : stmts) {
    stmt->pass();
  }

  set_scope(std::move(ctx->pop_scope()));
}


void ReturnStatement::pass() {
  expr->pass();
}


void MatchStatement::pass() {
  expr->pass();

  for (std::unique_ptr<MatchCase> &match : cases) {
    match->pass();
  }
}


void MatchCase::pass() {
  expr->pass();
  body->pass();
}


void AssignmentStatement::pass() {
  if (mut) {
    ctx->add_symb(ident, Symbol(SymbolKind::Variable, ctx->prev().meta));
  } else {
    ctx->add_symb(ident, Symbol(SymbolKind::Constant, ctx->prev().meta));
  }
}


void IfStatement::pass() {
  then_body->pass();
  else_body->pass();
}


void UntilStatement::pass() {
  body->pass();
}


void StructAST::pass() {
  // add struct to package symbol table
  ctx->add_symb(name, Symbol(TypeDefKind::TY_STRUCT, ctx->prev().meta));

  ctx->decl_scope();
  for (const std::pair<std::string, std::string> &field : fields) {
    if (ctx->get_symb(field.first)) {
      panic("duplicate field", field.first.c_str());
    }
    ctx->add_symb(field.first, Symbol(SymbolKind::Variable, ctx->prev().meta));
  }

  set_scope(std::move(ctx->pop_scope()));
}


void EnumAST::pass() {
  // add enum to package symbol table
  ctx->add_symb(name, Symbol(TypeDefKind::TY_ENUM, ctx->prev().meta));
}


void AbstractAST::pass() {
  // add abstract to package symbol table
  ctx->add_symb(name, Symbol(SymbolKind::Interface, ctx->prev().meta));
}


void ImplAST::pass() {
  // check that struct and abstract exist
  if (!ctx->get_symb(struct_name)) {
    panic("struct not found", struct_name.c_str());
  }

  if (!ctx->get_symb(abstract)) {
    panic("abstract not found", abstract.c_str());
  }

  // add every method to the struct symbol table
  //
  // this needs to get the linked scope from the struct symbol and push its scope onto the stack
  for (std::unique_ptr<FunctionAST> &def : defs) {
    def->pass();
  }
}


void DefaultExpr::pass() {
  // do nothing
}


void NullExpr::pass() {
  // do nothing
}


void VariableExpr::pass() {
  // do nothing
}


void FunctionCallExpr::pass() {
  for (std::unique_ptr<Expr> &arg : args) {
    arg->pass();
  }
}


void IntegerExpr::pass() {
  // do nothing
}


void FloatingPointExpr::pass() {
  // do nothing
}


void StringExpr::pass() {
  // do nothing
}


void CharExpr::pass() {
  // do nothing
}


void BinaryExpr::pass() {
  left_child->pass();
  right_child->pass();
}


void ByteExpr::pass() {
  // do nothing
}


void ByteStringExpr::pass() {
  // do nothing
}


void BoolExpr::pass() {
  // do nothing
}
