#include <string>
#include <vector>
#include <algorithm>

#include "../include/ast/Unit.h"
#include "../include/ast/Decl.h"
#include "../include/ast/Expr.h"
#include "../include/ast/Stmt.h"
#include "../include/core/Logger.h"
#include "../include/sema/ASTVisitor.h"

static std::vector<PackageUnit *> pkgs = {};
static bool has_entry = false;
static bool in_loop = false;
static std::shared_ptr<Scope> pkg_scope = nullptr;
static std::shared_ptr<Scope> impl_scope = nullptr;
static std::shared_ptr<Scope> top_scope = nullptr;
static const Type *fn_ret_type = nullptr;

/// This check verifies that a crate unit is valid. It checks that all packages
/// are unique and that the entry function 'main' exists.
void PassVisitor::visit(CrateUnit *u) {
  for (PackageUnit *pkg : u->get_packages()) {
    // check the package name is not duplicated
    for (PackageUnit *p : pkgs) {
      if (p->get_name() == pkg->get_name()) {
        panic("duplicate package: " + pkg->get_name());
      }
    }

    pkgs.push_back(pkg);
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
    if (import == u->get_name()) {
      panic("circular import: " + import);
    }

    bool found = false;
    for (const PackageUnit *pkg : pkgs) {
      if (pkg->get_name() == import) {
        found = true;
      }
    }

    if (!found) {
      panic("unresolved import: " + import + " in package: " + u->get_name());
    }

    if (std::count(imports.begin(), imports.end(), import) > 1) {
      panic("duplicate import: " + import + " in package: " + u->get_name());
    }
  }

  // import scope trees

  // TODO: check for duplicate decls
  for (const std::string &import : imports) {
    // get the pkg scope tree
    for (PackageUnit *pkg : pkgs) {
      if (pkg->get_name() == import) {
        for (Decl *decl : pkg->get_decls()) {
          if (decl->is_priv()) {
            continue;
          }

          if (NamedDecl *nd = dynamic_cast<NamedDecl *>(decl)) {
            u->get_scope()->add_decl(nd);
          }
        }
      }
    }
  }

  for (Decl *decl : u->get_decls()) {
    decl->pass(this);
    std::cout << decl->to_string() << '\n';
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
      panic("entry function 'main' cannot have parameters", d->get_meta());
    }

    // check return type is void
    if (d->get_type()) {
      panic("entry function 'main' must return void", d->get_meta());
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
      panic("unresolved return type: " + d->get_name(), d->get_meta());
    }

    StructDecl *struct_d = dynamic_cast<StructDecl *>(d->get_scope()->get_decl(T->get_ident()));
    if (!struct_d) {
      panic("unresolved return type: " + T->get_ident(), d->get_meta());
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
      panic("scoping error: " + d->get_name(), d->get_meta());
    }

    StructDecl *struct_d = dynamic_cast<StructDecl *>(top_scope->get_decl(T->get_ident()));
    if (!struct_d) {
      panic("unresolved parameter type: " + T->get_ident(), d->get_meta());
    }

    // assign real type
    if (struct_d->get_type()) {
      d->set_type(struct_d->get_type());
      return;
    }
  }
  panic("unresolved parameter type in scope: " + d->get_name(), d->get_meta());
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
      panic("scoping error: " + d->get_name(), d->get_meta());
    }

    StructDecl *struct_d = dynamic_cast<StructDecl *>(top_scope->get_decl(T->get_ident()));
    if (!struct_d) {
      panic("unresolved field type: " + T->get_ident(), d->get_meta());
    }

    // assign real type
    if (struct_d->get_type()) {
      d->set_type(struct_d->get_type());
      return;
    }
  }
  panic("unresolved field type in scope: " + d->get_name(), d->get_meta());
}


/// This check verifies that a trait declaration is valid. It passes on all
/// function declarations within the trait declaration.
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
  NamedDecl *decl = pkg_scope->get_decl(d->get_struct_name());
  if (!decl) {
    panic("unresolved decl: " + d->get_struct_name(), d->get_meta());
  }
  StructDecl *struct_d = dynamic_cast<StructDecl *>(decl);
  if (!struct_d) {
    panic("unresolved struct target: " + d->get_struct_name(), d->get_meta());
  }
  
  if (d->is_trait()) {
    // check that the trait exists
    Decl *gen_d = pkg_scope->get_decl(d->trait());
    if (!gen_d) {
      panic("unresolved trait: " + d->trait(), d->get_meta());
    }

    TraitDecl *trait_d = dynamic_cast<TraitDecl *>(gen_d);
    if (!trait_d) {
      panic("expected trait: " + d->trait(), d->get_meta());
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
        panic("missing trait implementation: " + fn->get_name(), d->get_meta());
      }
    }
  }

  for (FunctionDecl *fn : d->get_methods()) {
    // add the function to the struct scope
    struct_d->get_scope()->add_decl(fn);
  }

  impl_scope = struct_d->get_scope();
  for (FunctionDecl *fn : d->get_methods()) {
    fn->pass(this);
  }
  impl_scope = nullptr;
}


/// This check verifies that an enum declaration is valid. It passes on
/// all enum variants within the enum declaration.
void PassVisitor::visit(EnumDecl *d) {
  for (EnumVariantDecl *ev : d->get_variants()) {
    ev->pass(this);
  }
}


/// Empty pass on an enum variant declaration.
void PassVisitor::visit(EnumVariantDecl *d) {
  return;
}


/// This check verifies that a variable declaration has a valid type, and that
/// its type matches with that of the assigned expression, if it is not empty.
void PassVisitor::visit(VarDecl *d) {
  if (d->has_expr()) {
    d->get_expr()->pass(this);
  }
  
  if (!d->get_type()->is_builtin()) {
    // type is a reference
    if (const TypeRef *T = dynamic_cast<const TypeRef *>(d->get_type())) {
      if (!top_scope) {
        panic("scoping error: " + d->get_name(), d->get_meta());
      }

      StructDecl *struct_d = dynamic_cast<StructDecl *>(top_scope->get_decl(T->get_ident()));
      if (!struct_d) {
        panic("unresolved variable type: " + T->get_ident(), d->get_meta());
      }

      // assign real type
      if (struct_d->get_type()) {
        d->set_type(struct_d->get_type());
        return;
      }
    } else {
      panic("unresolved variable type in scope: " + d->get_name(), d->get_meta());
    } 
  }

  const PrimitiveType *pt = dynamic_cast<const PrimitiveType *>(d->get_type());
  if (!pt) {
    panic("unresolved variable type in scope: " + d->get_name(), d->get_meta());
  }

  if (!pt->compare(d->get_expr()->get_type())) {
    panic("type mismatch: " + d->get_name(), d->get_meta());
  }
}


/// This check verifies that a declaration statement is valid. It passes on the
/// nested declaration.
void PassVisitor::visit(DeclStmt *s) {
  s->get_decl()->pass(this);
}


/// This check verifies that a compound statement is valid. It passes on all
/// statements within the compound statement.
void PassVisitor::visit(CompoundStmt *s) {
  top_scope = s->get_scope();
  for (Stmt *stmt : s->get_stmts()) {
    stmt->pass(this);
  }
  top_scope = top_scope->get_parent();
}


/// This check verifies that an if statement is valid. It checks that the
/// condition is evaluable to a boolean, and that both the then and else
/// bodies are valid.
void PassVisitor::visit(IfStmt *s) {
  if (!s->get_cond()->get_type()->is_bool_evaluable()) {
    panic("non-boolean condition in if statement", s->get_meta());
  }

  s->get_cond()->pass(this);
  s->get_then_body()->pass(this);
  if (s->has_else()) {
    s->get_else_body()->pass(this);
  }
}


/// This check verifies that a match case is valid. It passes on both the
/// match expression and the match case.
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
    panic("non-matchable expression in match statement", s->get_meta());
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
    panic("missing true or false case in boolean match statement", s->get_meta());
  }

  if (!has_default && !is_bool_match) {
    panic("missing default case in match statement", s->get_meta());
  }
}


/// This check verifies that an until statement is valid. It checks that the
/// condition is evaluable to a boolean, and that the body is valid.
void PassVisitor::visit(UntilStmt *s) {
  s->get_cond()->pass(this);

  if (!s->get_cond()->get_type()->is_bool_evaluable()) {
    panic("non-boolean condition in until statement", s->get_meta());
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
    panic("return statement outside of function scope", s->get_meta());
  }

  if (!s->get_expr() && !fn_ret_type) {
    return;
  } else if (s->get_expr() && !fn_ret_type) {
    panic("return statement in void function", s->get_meta());
  }

  s->get_expr()->pass(this);
  if (fn_ret_type->is_builtin()) {
    const PrimitiveType *pt = dynamic_cast<const PrimitiveType *>(fn_ret_type);
    if (!pt->compare(s->get_expr()->get_type())) {
      panic("type mismatch in return statement", s->get_meta());
    }
  } else if (s->get_expr()->get_type() != fn_ret_type) {
    panic("type mismatch in return statement", s->get_meta());
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


/// This check verifies that a null expression is valid.
void PassVisitor::visit(NullExpr *e) {
  if (e->get_type()) {
    panic("non-null type in null expression", e->get_meta());
  }
}


/// This check verifies that a default expression is valid.
void PassVisitor::visit(DefaultExpr *e) {
  if (e->get_type()) {
    panic("non-null type in default expression", e->get_meta());
  }
}


/// This check verifies that a BooleanLiteral node is a boolean primitive.
void PassVisitor::visit(BooleanLiteral *e) {
  if (!e->get_type()->is_bool()) {
    panic("non-boolean type in boolean literal", e->get_meta());
  }
}


/// This check verifies that an IntegerLiteral node is an integer primitive.
void PassVisitor::visit(IntegerLiteral *e) {
  if (!e->get_type()->is_integer()) {
    panic("non-integer type in integer literal", e->get_meta());
  }
}


/// This check verifies that a FPLiteral node is a floating point primitive.
void PassVisitor::visit(FPLiteral *e) {
  if (!e->get_type()->is_float()) {
    panic("non-float type in floating point literal", e->get_meta());
  }
}


/// This check verifies that a CharLiteral node is a character primitive.
void PassVisitor::visit(CharLiteral *e) {
  if (!e->get_type()->is_char()) {
    panic("non-integer type in character literal", e->get_meta());
  }
}


/// This check verifies that a StringLiteral node is a string primitive.
void PassVisitor::visit(StringLiteral *e) {
  if (!e->get_type()->is_builtin()) {
    panic("non-primitive type in string literal", e->get_meta());
  }
}


/// This check verifies that a DeclRefExpr node is valid. It assigns the real
/// type of the declaration reference, assuming the node is valid.
void PassVisitor::visit(DeclRefExpr *e) {
  if (!e->get_type()->is_builtin()) {
    const TypeRef *T = dynamic_cast<const TypeRef *>(e->get_type());
    if (!T) {
      panic("unresolved reference type: " + e->get_ident(), e->get_meta());
    }

    StructDecl *struct_d = dynamic_cast<StructDecl *>(top_scope->get_decl(T->get_ident()));
    if (!struct_d) {
      panic("unresolved reference type: " + T->get_ident(), e->get_meta());
    }
    
    // assign real type
    if (struct_d->get_type()) {
      e->set_type(struct_d->get_type());
      return;
    }
  }
}


/// This check verifies that a BinaryExpr node is valid. It checks that both
/// the left and right hand sides of the expression are valid, and that their
/// types match. It also checks that the left hand side is a valid lvalue if
/// the operator is an assignment operator.
void PassVisitor::visit(BinaryExpr *e) {
  e->get_lhs()->pass(this);
  e->get_rhs()->pass(this);

  if (e->get_lhs()->get_type()->is_builtin() && e->get_rhs()->get_type()->is_builtin()) {
    const PrimitiveType *pt_lhs = dynamic_cast<const PrimitiveType *>(e->get_lhs()->get_type());
    const PrimitiveType *pt_rhs = dynamic_cast<const PrimitiveType *>(e->get_rhs()->get_type());
    if (!pt_lhs->compare(pt_rhs)) {
      panic("type mismatch in binary expression", e->get_meta());
    }
  } else if (e->get_lhs()->get_type() != e->get_rhs()->get_type()) {
    panic("type mismatch in binary expression", e->get_meta());
  }

  e->set_type(e->get_lhs()->get_type());

  if (is_assignment_op(e->get_op())) {
    // check that the left hand side is a valid lvalue
    if (DeclRefExpr *lhs = dynamic_cast<DeclRefExpr *>(e->get_lhs())) {
      // check that the left hand side is mutable
      if (VarDecl *vd = dynamic_cast<VarDecl *>(top_scope->get_decl(lhs->get_ident()))) {
        if (!vd->is_mut()) {
          panic("attempted to reassign immutable variable", e->get_meta());
        }
      }
    } else if (MemberExpr *lhs = dynamic_cast<MemberExpr *>(e->get_lhs())) {
      // check that the left hand side base is mutable
      if (DeclRefExpr *d = dynamic_cast<DeclRefExpr *>(lhs->get_base())) {
        Decl *gd = top_scope->get_decl(d->get_ident());
        if (!gd) {
          panic("unresolved reference: " + d->get_ident(), d->get_meta());
        }

        VarDecl *vd = dynamic_cast<VarDecl *>(gd);
        if (!vd) {
          panic("attempted to reassign non-variable identifier: " + d->get_ident(), e->get_meta());
        }
        
        if (!vd->is_mut()) {
          panic("attempted to reassign immutable variable", e->get_meta());
        }
      }
    } else {
      panic("assignment to non-lvalue", e->get_meta());
    }
  }
}


/// This check verifies that a unary expression is valid. It passes on the
/// nested expression and assigns the true type of the unary expression.
void PassVisitor::visit(UnaryExpr *e) {
  e->get_expr()->pass(this);
  e->set_type(e->get_expr()->get_type());

  if (e->is_bang() && !e->get_expr()->get_type()->is_bool()) {
    panic("non-boolean type in bang expression", e->get_meta());
  }
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
    const Type *real_type = nullptr;
    bool found = false;
    for (FieldDecl *fd : d->get_fields()) {
      if (fd->get_name() == f.first) {
        real_type = fd->get_type();
        found = true;
        break;
      }
    }

    if (!found) {
      panic("unknown field: " + f.first);
    }
    
    if (f.second->get_type()->is_builtin()) {
      const PrimitiveType *pt = dynamic_cast<const PrimitiveType *>(f.second->get_type());
      if (!pt->compare(real_type)) {
        panic("type mismatch in struct initialization: " + f.first, f.second->get_meta());
      }
    } else if (f.second->get_type() != real_type) {
      panic("type mismatch in struct initialization: " + f.first, f.second->get_meta());
    }

    f.second->pass(this);
  }

  // assign real type
  e->set_type(d->get_type());
}


/// This check verifies that a function call is valid. It checks that the callee
/// exists in the scope of the call, that all parameters are provided, and that
/// the return type of the call matches the return type of the callee.
void PassVisitor::visit(CallExpr *e) {
  // resolve function
  const std::string fn_name = e->get_callee();
  Decl *d = pkg_scope->get_decl(fn_name);
  if (!d) {
    panic("unresolved function: " + fn_name);
  }

  FunctionDecl *fn_d = dynamic_cast<FunctionDecl *>(d);
  if (!fn_d) {
    panic("expected function: " + fn_name);
  }

  if (e->get_num_args() != fn_d->get_num_params()) {
    panic("function " + fn_name + " has " + std::to_string(fn_d->get_num_params()) + " parameters but " + \
    std::to_string(e->get_num_args()) + " were provided.");
  }

  // type check all arguments
  if (fn_d->has_params()) {
    std::size_t pos = 0;
    for (ParamVarDecl *param : fn_d->get_params()) {
      param->pass(this);
      Expr *arg = e->get_arg(pos);
      if (!arg) {
        panic("missing argument in function call: " + param->get_name());
      }
      arg->pass(this);

      if (param->get_type()->is_builtin()) {
        const PrimitiveType *pt = dynamic_cast<const PrimitiveType *>(param->get_type());
        if (!pt->compare(arg->get_type())) {
          panic("type mismatch in function call: " + param->get_name());
        }
      } else if (param->get_type() != arg->get_type()) {
        panic("type mismatch in function call: " + param->get_name());
      }

      pos += 1;
    }
  }

  // check if the function return is a type reference
  if (const TypeRef *T = dynamic_cast<const TypeRef *>(fn_d->get_type())) {
    // check if the referenced type exists
    StructDecl *struct_d = dynamic_cast<StructDecl *>(pkg_scope->get_decl(T->get_ident()));
    if (!struct_d) {
      panic("unresolved return type: " + T->get_ident());
    }

    // assign real type
    if (struct_d->get_type()) {
      e->set_type(struct_d->get_type());
      return;
    }
    panic("unresolved return type: " + T->get_ident());
  }

  // assign real type
  e->set_type(fn_d->get_type());
}


/// This check verifies that a member access expression is valid. It checks that
/// the base expression is a struct, and that the member exists in the struct.
void PassVisitor::visit(MemberExpr *e) {
  // null base validation
  if (!e->get_base()) {
    panic("member access on null expression", e->get_meta());
  }
  e->get_base()->pass(this);

  if (e->get_base()->get_type()->is_builtin()) {
    panic("member access on non-struct type", e->get_meta());
  }

  // resolve struct type from base type
  const StructType *st = dynamic_cast<const StructType *>(e->get_base()->get_type());
  if (!st) {
    panic("expected struct type", e->get_meta());
  }

  // resolve generic struct declaration from package scope
  const std::string struct_name = st->get_name();
  Decl *d = pkg_scope->get_decl(struct_name);
  if (!d) {
    panic("unresolved declaration type: " + struct_name, e->get_meta());
  }

  // resolve specific struct declaration from package scope
  StructDecl *struct_d = dynamic_cast<StructDecl *>(d);
  if (!struct_d) {
    panic("expected struct: " + struct_name, e->get_meta());
  }

  // resolve target field declaration from struct scope
  FieldDecl *fd = struct_d->get_field(e->get_member());
  if (!fd) {
    panic("unresolved field: " + e->get_member(), e->get_meta());
  }

  // access level check
  if (fd->is_priv() && impl_scope != struct_d->get_scope()) {
    panic("attempted to access private field: " + e->get_member(), e->get_meta());
  }

  // assign real type
  e->set_type(fd->get_type());
}


/// This check verifies that a member call expression is valid. It checks that
/// the base expression is a struct, and that the methods exists in the struct.
void PassVisitor::visit(MemberCallExpr *e) {
  // null base validation
  if (!e->get_base()) {
    panic("member access on null expression", e->get_meta());
  }
  e->get_base()->pass(this);

  // resolve base type
  const Type *base_type = e->get_base()->get_type();
  if (base_type->is_builtin()) {
    panic("member access on non-struct type", e->get_meta());
  }

  // resolve struct type from base type
  const StructType *st = dynamic_cast<const StructType *>(base_type);
  if (!st) {
    panic("expected struct type", e->get_meta());
  }

  // resolve generic struct declaration from package scope
  const std::string struct_name = st->get_name();
  Decl *d = pkg_scope->get_decl(struct_name);
  if (!d) {
    panic("unresolved declaration type: " + struct_name, e->get_meta());
  }

  // resolve specific struct declaration from package scope
  StructDecl *struct_d = dynamic_cast<StructDecl *>(d);
  if (!struct_d) {
    panic("expected struct: " + struct_name, e->get_meta());
  }

  // resolve target generic declaration from struct scope
  NamedDecl *md = struct_d->get_scope()->get_decl(e->get_callee());
  if (!md) {
    panic("unresolved method: " + e->get_callee(), e->get_meta());
  }

  // resolve target specific method declaration from struct scope
  FunctionDecl *method_decl = dynamic_cast<FunctionDecl *>(md);
  if (!method_decl) {
    panic("expected function: " + e->get_callee());
  }

  // access level check
  if (method_decl->is_priv() && impl_scope != struct_d->get_scope()) {
    panic("attempted to access private method: " + e->get_callee(), e->get_meta());
  }

  // param count check
  if (e->get_num_args() != method_decl->get_num_params()) {
    panic("function " + e->get_callee() + " has " + std::to_string(method_decl->get_num_params()) + " parameters but " + \
    std::to_string(e->get_num_args()) + " were provided.");
  }

  // type check all arguments
  if (method_decl->has_params()) {
    std::size_t pos = 0;
    for (ParamVarDecl *param : method_decl->get_params()) {
      param->pass(this);
      Expr *arg = e->get_arg(pos);
      if (!arg) {
        panic("missing argument in function call: " + param->get_name());
      }
      arg->pass(this);

      if (param->get_type()->is_builtin()) {
        const PrimitiveType *pt = dynamic_cast<const PrimitiveType *>(param->get_type());
        if (!pt->compare(arg->get_type())) {
          panic("type mismatch in function call: " + param->get_name());
        }
      } else if (param->get_type() != arg->get_type()) {
        panic("type mismatch in function call: " + param->get_name());
      }

      pos += 1;
    }
  }

  // check if the function return is a type reference
  if (const TypeRef *T = dynamic_cast<const TypeRef *>(method_decl->get_type())) {
    // check if the referenced type exists
    StructDecl *struct_d = dynamic_cast<StructDecl *>(pkg_scope->get_decl(T->get_ident()));
    if (!struct_d) {
      panic("unresolved return type: " + T->get_ident());
    }

    // assign real type
    if (struct_d->get_type()) {
      e->set_type(struct_d->get_type());
      return;
    }
    panic("unresolved return type: " + T->get_ident());
  }

  // assign real type
  e->set_type(method_decl->get_type());
}


/// This check resolves the real type of a this expression.
void PassVisitor::visit(ThisExpr *e) {
  if (!top_scope) {
    panic("this expression outside of struct scope", e->get_meta());
  }

  const TypeRef *RT = dynamic_cast<const TypeRef *>(e->get_type());
  if (!RT) {
    panic("unresolved 'this' type", e->get_meta());
  }

  StructDecl *struct_d = dynamic_cast<StructDecl *>(top_scope->get_decl(RT->get_ident()));
  if (!struct_d) {
    panic("unresolved 'this' type", e->get_meta());
  }

  if (struct_d->get_type()) {
    e->set_type(struct_d->get_type());
  }
}
