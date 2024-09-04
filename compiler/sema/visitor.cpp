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


  for (std::unique_ptr<Decl> &decl : u->get_decls()) {
    decl->pass(this);
  }
}


