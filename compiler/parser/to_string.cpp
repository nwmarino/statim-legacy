#include "../include/ast/Decl.h"
#include "../include/ast/Expr.h"
#include "../include/ast/Stmt.h"
#include "../include/ast/Unit.h"

static const std::string mk_piping(int n ) {
  std::string result;
  for (int i = 0; i < n; i += 2) {
    result += '|' + std::string(2, ' ');
  }
  return result;
}


const std::string CrateUnit::to_string(int n) {
  std::string result;
  for (std::unique_ptr<PackageUnit> const &package : packages) {
    result += package->to_string(n);
  }
  return result;
}


const std::string PackageUnit::to_string(int n) {
  std::string result = "PackageUnit " + name + '\n';
  for (std::unique_ptr<Decl> const &decl : decls) {
    result += decl->to_string(n + 2);
  }
  return result;
}


const std::string FunctionDecl::to_string(int n) {
  std::string result = is_priv() ? mk_piping(n) + "FunctionDecl " + name + " `private`\n" : mk_piping(n) + "FunctionDecl " + name + '\n';
  if (has_body()) {
    result += body->to_string(n + 2);
  }
  return result;
}


const std::string FieldDecl::to_string(int n) {
  return is_priv() ? mk_piping(n) + "FieldDecl " + name + " `private`\n" : mk_piping(n) + "FieldDecl " + name + '\n';
}


const std::string StructDecl::to_string(int n) {
  std::string result = is_priv() ? mk_piping(n) + "StructDecl " + name + " `private`\n" : mk_piping(n) + "StructDecl " + name + '\n';
  for (std::unique_ptr<FieldDecl> const &field : fields) {
    result += field->to_string(n + 2);
  }
  return result;
}


const std::string ImplDecl::to_string(int n) {
  std::string result = mk_piping(n) + "ImplDecl " + trait() + '\n';
  for (std::unique_ptr<FunctionDecl> const &method : methods) {
    result += method->to_string(n + 2);
  }
  return result;
}


const std::string EnumVariant::to_string(int n) {
  return mk_piping(n) + "EnumVariant " + name + '\n';
}


const std::string EnumDecl::to_string(int n) {
  std::string result = is_priv() ? mk_piping(n) + "EnumDecl " + name + " `private`\n" : mk_piping(n) + "EnumDecl " + name + '\n';
  for (EnumVariant &variant : variants) {
    result += variant.to_string(n + 2);
  }
  return result;
}


const std::string TraitDecl::to_string(int n) {
  std::string result = is_priv() ? mk_piping(n) + "TraitDecl " + name + " `private`\n" : mk_piping(n) + "TraitDecl " + name + '\n';
  for (std::unique_ptr<FunctionDecl> &decl : decls) {
    result += decl->to_string(n + 2);
  }
  return result;
}


const std::string VarDecl::to_string(int n) {
  std::string result = is_mut() ? mk_piping(n) + "VarDecl " + name + " `mut`\n" : mk_piping(n) + "VarDecl " + name + '\n';
  if (has_expr()) {
    result += expr->to_string(n + 2);
  }
  return result;
}


const std::string DeclStmt::to_string(int n) {
  return mk_piping(n) + "DeclStmt\n" + decl->to_string(n + 2);
}


const std::string CompoundStmt::to_string(int n) {
  std::string result = mk_piping(n) + "CompoundStmt\n";
  for (std::unique_ptr<Stmt> const &stmt : stmts) {
    result += stmt->to_string(n + 2);
  }
  result += scope->to_string(n);
  return result;
}


const std::string ReturnStmt::to_string(int n) {
  return mk_piping(n) + "ReturnStmt\n" + expr->to_string(n + 2);
}


const std::string IfStmt::to_string(int n) {
  std::string result = mk_piping(n) + "IfStmt\n";
  result += cond->to_string(n + 2);
  result += then_body->to_string(n + 2);
  if (else_body) {
    result += else_body->to_string(n + 2);
  }
  return result;
}


const std::string MatchCase::to_string(int n) {
  std::string result = mk_piping(n) + "MatchCase\n";
  result += expr->to_string(n + 2);
  result += body->to_string(n + 2);
  return result;
}


const std::string MatchStmt::to_string(int n) {
  std::string result = mk_piping(n) + "MatchStmt\n";
  result += expr->to_string(n + 2);
  for (std::unique_ptr<MatchCase> const &match_case : cases) {
    result += match_case->to_string(n + 2);
  }
  return result;
}


const std::string UntilStmt::to_string(int n) {
  std::string result = mk_piping(n) + "UntilStmt\n";
  result += cond->to_string(n + 2);
  result += body->to_string(n + 2);
  return result;
}


const std::string DefaultExpr::to_string(int n) {
  return mk_piping(n) + "DefaultExpr\n";
}


const std::string NullExpr::to_string(int n) {
  return mk_piping(n) + "NullExpr\n";
}


const std::string BooleanExpr::to_string(int n) {
  return mk_piping(n) + "BooleanExpr " + std::to_string(value) + '\n';
}


const std::string IntegerExpr::to_string(int n) {
  return mk_piping(n) + "IntegerExpr " + std::to_string(value) + '\n';
}


const std::string FloatingPointExpr::to_string(int n) {
  return mk_piping(n) + "FloatingPointExpr " + std::to_string(value) + '\n';
}


const std::string CharExpr::to_string(int n) {
  return mk_piping(n) + "CharExpr " + std::to_string(value) + '\n';
}


const std::string StringExpr::to_string(int n) {
  return mk_piping(n) + "StringExpr " + value + '\n';
}


const std::string BinaryExpr::to_string(int n) {
  std::string result = mk_piping(n) + "BinaryExpr " + op + '\n';
  result += lhs->to_string(n + 2);
  result += rhs->to_string(n + 2);
  return result;
}


const std::string CallExpr::to_string(int n) {
  return mk_piping(n) + "CallExpr " + callee + '\n';
}


const std::string VariableExpr::to_string(int n) {
  return mk_piping(n) + "VariableExpr " + ident + '\n';
}


const std::string ByteExpr::to_string(int n) {
  return mk_piping(n) + "ByteExpr " + std::to_string(value) + '\n';
}


const std::string ByteStringExpr::to_string(int n) {
  return mk_piping(n) + "ByteStringExpr " + value + '\n';
}


const std::string Scope::to_string(int n) {
  std::string result = mk_piping(n) + "Scope ";
  if (is_struct_scope()) {
    result += "`struct`";
  } else if (is_compound_scope()) {
    result += "`compound`";
  } else if (is_cond_scope()) {
    result += "`conditional`";
  } else if (is_decl_scope()) {
    result += "`declaration`";
  } else if (is_func_scope()) {
    result += "`function`";
  } else if (is_loop_scope()) {
    result += "`loop`";
  } else if (is_pkg_scope()) {
    result += "`package`";
  }
  result += '\n';
  /*
  for (Decl *d : decls) {
    result += d->to_string(n + 2);
  }
  */
  if (parent) {
    result += parent->to_string(n - 2);
  }
  return result;
}