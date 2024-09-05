#include <string>
#include <vector>

#include "../include/ast/Unit.h"
#include "../include/ast/Decl.h"
#include "../include/ast/Expr.h"
#include "../include/ast/Stmt.h"
#include "../include/core/Logger.h"
#include "../include/sema/ASTVisitor.h"

static std::vector<std::string> pkg_ids = {};
static bool has_entry = false;

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
    pkg->pass(this);
  }

  if (!has_entry) {
    panic("no entry function 'main' found");
  }
}


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
  }
}


void PassVisitor::visit(FunctionDecl *d) {
  // check for duplicate declaration
  if (d->get_scope()->get_decl(d->get_name())) {
    panic("symbol already exists in scope: " + d->get_name());
  }

  // check for entry function
  if (d->get_name() == "main") {
    // check no params exist
    if (d->has_params()) {
      panic("entry function 'main' cannot have parameters");
    }

    // check return type is void
    if (d->get_ret_type() != "void") {
      panic("entry function 'main' must return void");
    }

    has_entry = true;
  }

  // check that each param type exists in this scope
  for (ParamVarDecl *param : d->get_params()) {
    if (!d->get_scope()->get_decl(param->get_type())) {
      panic("unresolved parameter type: " + param->get_type());
    }
  }
  
  // check that a return type exists
  if (d->get_ret_type() != "void" && !d->get_scope()->get_decl(d->get_ret_type())) {
    panic("unresolved return type: " + d->get_ret_type());
  }

  if (Stmt *s = d->get_body()) {
    s->pass(this);
  }
}

