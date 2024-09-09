#include <string>
#include <vector>
#include <algorithm>

#include "../include/ast/Unit.h"
#include "../include/ast/Decl.h"
#include "../include/ast/Expr.h"
#include "../include/ast/Stmt.h"
#include "../include/core/Logger.h"
#include "../include/sema/ASTVisitor.h"

static std::vector<std::string> pkg_ids = {};
static bool has_entry = false;
static bool in_loop = false;
static std::shared_ptr<Scope> pkg_scope = nullptr;
static std::shared_ptr<Scope> top_scope = nullptr;
static const Type *fn_ret_type = nullptr;

/// This check verifies that a crate unit is valid. It checks that all packages
/// are unique and that the entry function 'main' exists.
void PassVisitor::visit(CrateUnit *u) {
  std::vector <PackageUnit *> pkgs = u->get_packages();
  for (PackageUnit *pkg : pkgs) {
    // check the package name is not duplicated
    if (std::find(pkg_ids.begin(), pkg_ids.end(), pkg->get_name()) != pkg_ids.end()) {
      panic("duplicate package: " + pkg->get_name());
    }
    pkg_ids.push_back(pkg->get_name());
  }

  for (PackageUnit *pkg : u->get_packages()) {
    pkg_scope = pkg->get_scope();
    pkg->pass(this);
  }
  pkg_scope = nullptr;

  if (!has_entry) {
    panic("no entry function 'main' found");
  }
}


/// This check verifies that a package unit is valid. It checks that all imports
/// are resolved and that no duplicates exist. It also checks that all declarations
/// in the package are valid.
void PassVisitor::visit(PackageUnit *u) {
  std::vector<std::string> imports = u->get_imports();

  // check all imports exist and that no dupes exist
  for (const std::string &import : imports) {
    if (std::find(pkg_ids.begin(), pkg_ids.end(), import) == pkg_ids.end()) {
      panic("unresolved import: " + import);
    }

    if (std::count(imports.begin(), imports.end(), import) > 1) {
      panic("duplicate import: " + import);
    }
  }

  // import scope trees


  for (Decl *decl : u->get_decls()) {
    decl->pass(this);
    std::cout << "Passed decl: " + decl->get_name() + '\n'; // debugging
  }
}


/// This check verifies that a function declaration has a valid return type.
/// If the function is the entry point, it also verifies the semantics of it,
/// in particular that no return type or arguments exist.
void PassVisitor::visit(FunctionDecl *d) {
  // check for entry function
  if (d->is_main()) {
    // check no params exist
    if (d->has_params()) {
      panic("entry function 'main' cannot have parameters");
    }

    // check return type is void
    if (d->get_type()) {
      panic("entry function 'main' must return void");
    }

    has_entry = true;
  }

  // check that each param type exists in this scope
  top_scope = d->get_scope();
  for (ParamVarDecl *param : d->get_params()) {
    param->pass(this);
  }
  
  // check that a valid return type exists
  if (d->get_type() && !d->get_type()->is_builtin()) {
    const TypeRef *T = dynamic_cast<const TypeRef *>(d->get_type());
    if (!T) {
      panic("unresolved return type: " + d->get_name());
    }

    StructDecl *struct_d = dynamic_cast<StructDecl *>(d->get_scope()->get_decl(T->get_ident()));
    if (!struct_d) {
      panic("unresolved return type: " + T->get_ident());
    }

    // assign real type
    if (struct_d->get_type()) {
      d->set_type(struct_d->get_type());
    }
  }

  fn_ret_type = d->get_type();
  if (Stmt *s = d->get_body()) {
    s->pass(this);
  }
  fn_ret_type = nullptr;
  top_scope = nullptr;
}


/// This check verifies that a parameter declaration in a function has a valid type.
void PassVisitor::visit(ParamVarDecl *d) {
  if (d->get_type()->is_builtin()) {
    return;
  }

  // type is a reference
  if (const TypeRef *T = dynamic_cast<const TypeRef *>(d->get_type())) {
    if (!top_scope) {
      panic("scoping error: " + d->get_name());
    }

    StructDecl *struct_d = dynamic_cast<StructDecl *>(top_scope->get_decl(T->get_ident()));
    if (!struct_d) {
      panic("unresolved parameter type: " + T->get_ident());
    }

    // assign real type
    if (struct_d->get_type()) {
      d->set_type(struct_d->get_type());
      return;
    }
  }
  panic("unresolved parameter type in scope: " + d->get_name());
}


/// This check facilitaties the verification of struct declarations.
void PassVisitor::visit(StructDecl *d) {
  // check that each field type exists in this scope
  top_scope = d->get_scope();
  for (FieldDecl *field : d->get_fields()) {
    field->pass(this);
  }
  top_scope = nullptr;
}


/// This check verifies that a field declaration in a struct has a valid type.
void PassVisitor::visit(FieldDecl *d) {
  if (d->get_type()->is_builtin()) {
    return;
  }

  // type is a reference
  if (const TypeRef *T = dynamic_cast<const TypeRef *>(d->get_type())) {
    if (!top_scope) {
      panic("scoping error: " + d->get_name());
    }

    StructDecl *struct_d = dynamic_cast<StructDecl *>(top_scope->get_decl(T->get_ident()));
    if (!struct_d) {
      panic("unresolved field type: " + T->get_ident());
    }

    // assign real type
    if (struct_d->get_type()) {
      d->set_type(struct_d->get_type());
      return;
    }
  }
  panic("unresolved field type in scope: " + d->get_name());
}


void PassVisitor::visit(TraitDecl *d) {
  for (FunctionDecl *fn : d->get_decls()) {
    fn->pass(this);
  }
}


/// This check verifies that both the target struct and trait exist in the
/// scope of the declaration. Then, it asserts that all trait behaviour is
/// implemented to the target struct.
void PassVisitor::visit(ImplDecl *d) {
  // check that the target struct exists
  if (!pkg_scope->get_decl(d->get_name())) {
    panic("unresolved struct: " + d->get_name());
  }
  
  if (d->is_trait()) {
    // check that the trait exists
    Decl *gen_d = pkg_scope->get_decl(d->trait());
    if (!gen_d) {
      panic("unresolved trait: " + d->trait());
    }

    TraitDecl *trait_d = dynamic_cast<TraitDecl *>(gen_d);
    if (!trait_d) {
      panic("expected trait: " + d->trait());
    }

    // ensure that all methods are implemented
    for (FunctionDecl *fn : trait_d->get_decls()) {
      bool found = false;
      for (FunctionDecl *impl_fn : d->get_methods()) {
        if (fn->get_name() == impl_fn->get_name() && fn->get_type() == impl_fn->get_type()) {
          found = true;
          break;
        }
      }

      if (!found) {
        panic("missing trait implementation: " + fn->get_name());
      }
    }
  }

  for (FunctionDecl *fn : d->get_methods()) {
    fn->pass(this);
  }
}


void PassVisitor::visit(EnumDecl *d) {
  for (EnumVariantDecl *ev : d->get_variants()) {
    ev->pass(this);
  }
}


void PassVisitor::visit(EnumVariantDecl *d) {
  return;
}


/// This check verifies that a variable declaration has a valid type, and that
/// its type matches with that of the assigned expression, if it is not empty.
void PassVisitor::visit(VarDecl *d) {
  d->get_expr()->pass(this);
  if (!d->get_type()->is_builtin()) {
    // type is a reference
    if (const TypeRef *T = dynamic_cast<const TypeRef *>(d->get_type())) {
      if (!top_scope) {
        panic("scoping error: " + d->get_name());
      }

      StructDecl *struct_d = dynamic_cast<StructDecl *>(top_scope->get_decl(T->get_ident()));
      if (!struct_d) {
        panic("unresolved variable type: " + T->get_ident());
      }

      // assign real type
      if (struct_d->get_type()) {
        d->set_type(struct_d->get_type());
        return;
      }
    } else {
      panic("unresolved variable type in scope: " + d->get_name());
    } 
  }

  const PrimitiveType *pt = dynamic_cast<const PrimitiveType *>(d->get_type());
  if (!pt) {
    panic("unresolved variable type in scope: " + d->get_name());
  }

  if (!pt->compare(d->get_expr()->get_type())) {
    panic("type mismatch: " + d->get_name());
  }
}


void PassVisitor::visit(DeclStmt *s) {
  s->get_decl()->pass(this);
}


void PassVisitor::visit(CompoundStmt *s) {
  for (Stmt *stmt : s->get_stmts()) {
    stmt->pass(this);
  }
}


/// This check verifies that an if statement is valid. It checks that the
/// condition is evaluable to a boolean, and that both the then and else
/// bodies are valid.
void PassVisitor::visit(IfStmt *s) {
  if (!s->get_cond()->get_type()->is_bool_evaluable()) {
    panic("non-boolean condition in if statement");
  }

  s->get_cond()->pass(this);
  s->get_then_body()->pass(this);
  if (s->has_else()) {
    s->get_else_body()->pass(this);
  }
}


void PassVisitor::visit(MatchCase *s) {
  s->get_expr()->pass(this);
  s->get_body()->pass(this);
}


/// This check verifies that a match statement is valid. It checks that the
/// target expression is evaluable to a type that can be matched, and that
/// all cases are valid. Additionally, a default case must exist, unless the
/// expression is a boolean, in which case both true and false cases must exist.
void PassVisitor::visit(MatchStmt *s) {
  s->get_expr()->pass(this);
  if (!s->get_expr()->get_type()->is_matchable()) {
    panic("non-matchable expression in match statement");
  }

  bool is_bool_match = false;
  if (s->get_expr()->get_type()->is_bool()) {
    is_bool_match = true;
  }

  bool has_default = false;
  bool has_true_case = false;
  bool has_false_case = false;
  for (MatchCase *c : s->get_cases()) {
    c->pass(this);

    if (!c->get_expr()->get_type()) {
      has_default = true;
    }

    if (BooleanLiteral *bl = dynamic_cast<BooleanLiteral *>(c->get_expr())) {
      if (bl->get_value()) {
        has_true_case = true;
      } else {
        has_false_case = true;
      }
    }
  }

  if (is_bool_match && (!has_true_case || !has_false_case)) {
    panic("missing true or false case in boolean match statement");
  }

  if (!has_default && !is_bool_match) {
    panic("missing default case in match statement");
  }
}


/// This check verifies that an until statement is valid. It checks that the
/// condition is evaluable to a boolean, and that the body is valid.
void PassVisitor::visit(UntilStmt *s) {
  s->get_cond()->pass(this);

  if (!s->get_cond()->get_type()->is_bool_evaluable()) {
    panic("non-boolean condition in until statement");
  }

  in_loop = true;
  s->get_body()->pass(this);
  in_loop = false;
}


/// This check verifies that a return statement is valid. It checks that the
/// expression, if it exists, is equivelant to the return type of the function.
void PassVisitor::visit(ReturnStmt *s) {
  // check that the return stmt is in a function scope
  if (!top_scope) {
    panic("return statement outside of function scope");
  }

  if (!s->get_expr() && !fn_ret_type) {
    return;
  } else if (s->get_expr() && !fn_ret_type) {
    panic("return statement in void function");
  }

  s->get_expr()->pass(this);
  if (s->get_expr()->get_type() != fn_ret_type) {
    panic("type mismatch in return statement");
  }
}


/// This check verifies that a break statement is in the scope of a loop.
void PassVisitor::visit(BreakStmt *s) {
  if (!in_loop) {
    panic("break statement outside of loop scope");
  }
}


/// This check verifies that a continue statement is in the scope of a loop.
void PassVisitor::visit(ContinueStmt *s) {
  if (!in_loop) {
    panic("continue statement outside of loop scope");
  }
}


void PassVisitor::visit(NullExpr *e) {
  if (e->get_type()) {
    panic("non-null type in null expression");
  }
}


void PassVisitor::visit(DefaultExpr *e) {
  if (e->get_type()) {
    panic("non-null type in default expression");
  }
}


void PassVisitor::visit(BooleanLiteral *e) {
  if (!e->get_type()->is_bool()) {
    panic("non-boolean type in boolean literal");
  }
}


void PassVisitor::visit(IntegerLiteral *e) {
  if (!e->get_type()->is_integer()) {
    panic("non-integer type in integer literal");
  }
}


void PassVisitor::visit(FPLiteral *e) {
  if (!e->get_type()->is_float()) {
    panic("non-float type in floating point literal");
  }
}


void PassVisitor::visit(CharLiteral *e) {
  if (!e->get_type()->is_integer()) {
    panic("non-integer type in character literal");
  }
}


void PassVisitor::visit(StringLiteral *e) {
  if (!e->get_type()->is_builtin()) {
    panic("non-primitive type in string literal");
  }
}


void PassVisitor::visit(DeclRefExpr *e) {
  return;
}


void PassVisitor::visit(BinaryExpr *e) {
  return;
}


void PassVisitor::visit(UnaryExpr *e) {
  return;
}


/// This check verifies that a struct initialization expression is valid.
/// It checks that all fields initialized exist in the struct, and that
/// their types correspond.
void PassVisitor::visit(InitExpr *e) {
  // resolve target struct
  StructDecl *d = dynamic_cast<StructDecl *>(pkg_scope->get_decl(e->get_ident()));
  if (!d) {
    panic("unresolved struct: " + e->get_ident());
  }

  // check each field is valid
  for (const std::pair<std::string, Expr *> f : e->get_fields()) {
    // check that the field exists in the struct
    for (const FieldDecl *fd : d->get_fields()) {
      if (fd->get_name() == f.first) {
        break;
      }
      panic("unknown field: " + f.first);
    }

    // check that the type of the field is correct
    if (!f.second->get_type()->is_builtin()) {
      // type is a reference
      if (const TypeRef *T = dynamic_cast<const TypeRef *>(f.second->get_type())) {
        if (!top_scope) {
          panic("scoping error: " + f.first);
        }

        StructDecl *struct_d = dynamic_cast<StructDecl *>(top_scope->get_decl(T->get_ident()));
        if (!struct_d) {
          panic("unresolved field type: " + T->get_ident());
        }

        // assign real type
        if (const TypeRef *T = dynamic_cast<const TypeRef *>(f.second->get_type())) {
          if (struct_d->get_type()) {
            T->set_type(struct_d->get_type());
          }
        } else {
          panic("unresolved field type in scope: " + f.first);
        }
      } else {
        panic("unresolved field type in scope: " + f.first);
      }
    }

    f.second->pa ss(this);
  }
}


void PassVisitor::visit(CallExpr *e) {
  return;
}


void PassVisitor::visit(MemberExpr *e) {
  return;
}


void PassVisitor::visit(MemberCallExpr *e) {
  return;
}


void PassVisitor::visit(ArrayAccessExpr *e) {
  return;
}
