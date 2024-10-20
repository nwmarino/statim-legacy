/// This source file houses the code for the ast printer.

#include "../include/ast/Decl.h"
#include "../include/ast/Expr.h"
#include "../include/ast/Stmt.h"
#include "../include/ast/Unit.h"

static int indent = 0;
static bool at_last_child = false;
static std::vector<int> place_vert = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

static const std::string RESET = "\033[0m";
static const std::string RED = "\033[31m";
static const std::string GREEN = "\033[32m";
static const std::string YELLOW = "\033[33m";
static const std::string BLUE = "\033[34m";
static const std::string MAGENTA = "\033[35m";
static const std::string CYAN = "\033[36m";
static const std::string WHITE = "\033[37m";
static const std::string BOLD = "\033[1m";

/// Returns a string representation of piping dependent on indent, place_vert, and at_last_child.
static const std::string piping() {
  std::string result = BLUE;
  for (int i = 0; i < indent; i++) {
    result = place_vert[i] ? result + "| " : result + "  ";
  }
  return at_last_child ? result + "`-" + RESET : result + "|-" + RESET;
}


const std::string CrateUnit::to_string() {
  std::string result;
  for (std::unique_ptr<PackageUnit> const &package : packages) {
    at_last_child = package == packages.back();
    result += package->to_string() + '\n';
  }
  at_last_child = false;
  return result;
}


const std::string PackageUnit::to_string() {
  std::string result = BOLD + YELLOW + "PackageUnit " + RESET + BLUE + name + RESET;
  if (imports.size() > 0) {
    result += " << ";
    for (const std::string &import : imports) {
      result += YELLOW + import + " " + RESET;
    }
  }
  result += '\n';
  place_vert[indent] = 1;
  for (std::unique_ptr<Decl> const &decl : decls) {
    at_last_child = decl == decls.back();
    place_vert[indent] = at_last_child ? 0 : 1;
    result += decl->to_string();
  }
  at_last_child = false;
  return result + RESET;
}


const std::string FunctionDecl::to_string() {
  const std::string type = get_type() ? get_type()->to_string() : "void";
  std::string result = piping() + BOLD + RED + "FunctionDecl" + RESET + GREEN + " '" + type + "' " + BLUE + name + RESET;
  result = is_priv() ? result + " private\n" : result + '\n'; 
  indent++;
  for (std::unique_ptr<ParamVarDecl> &param : params) {
    result += param->to_string();
  }
  if (has_body()) {
    at_last_child = true;
    result += body->to_string();
  }
  indent = 0;
  return result;
}


const std::string ParamVarDecl::to_string() {
  return piping() + RED + "ParamVarDecl" + GREEN + " '" + get_type()->to_string() + "' " + BLUE + name + RESET + '\n';
}


const std::string FieldDecl::to_string() {
  std::string result = piping() + RED + "FieldDecl" + GREEN + " '" + get_type()->to_string() + "' " + BLUE + name + RESET;
  if (is_priv()) {
    result += " private\n";
  } else {
    result += '\n';
  }
  return result;
}


const std::string StructDecl::to_string() {
  std::string result = is_priv() ? piping() + BOLD + RED + "StructDecl " + RESET + GREEN + name + RESET + " private\n" : \
    piping() + BOLD + RED + "StructDecl " + RESET + BLUE + name + RESET + '\n';
  indent++;
  for (std::unique_ptr<FieldDecl> const &field : fields) {
    at_last_child = field == fields.back();
    result += field->to_string();
  }
  indent = 0;
  return result;
}


const std::string ImplDecl::to_string() {
  std::string result = piping() + BOLD + RED + "ImplDecl" + RESET;
  if (is_trait()) {
    result += YELLOW + " '" + trait() + "'" + RESET;
  }
  result += BLUE + " '" + _struct + "' " + RESET + '\n';
  indent++;
  for (std::unique_ptr<FunctionDecl> const &method : methods) {
    at_last_child = method == methods.back();
    result += method->to_string();
  }
  indent = 0;
  return result;
}


const std::string EnumVariantDecl::to_string() {
  return piping() + RED + "EnumVariantDecl " + BLUE + name + RESET + '\n';
}


const std::string EnumDecl::to_string() {
  std::string result = is_priv() ? piping() + BOLD + RED + "EnumDecl " + RESET + BLUE + name + RESET + " private\n" : \
    piping() + BOLD + RED + "EnumDecl " + RESET + BLUE + name + RESET + '\n';
  indent++;
  for (const std::unique_ptr<EnumVariantDecl> &variant : variants) {
    at_last_child = &variant == &variants.back();
    result += variant->to_string();
  }
  indent = 0;
  return result;
}


const std::string TraitDecl::to_string() {
  std::string result = is_priv() ? piping() + BOLD + RED + "TraitDecl " + RESET + BLUE + name + RESET + " private\n" : \
    piping() + BOLD + RED + "TraitDecl " + RESET + BLUE + name + RESET + '\n';
  indent++;
  for (std::unique_ptr<FunctionDecl> &decl : decls) {
    at_last_child = decl == decls.back();
    result += decl->to_string();
  }
  indent = 0;
  return result;
}


const std::string VarDecl::to_string() {
  std::string result = piping() + RED + "VarDecl" + GREEN + " '" + get_type()->to_string() + "' " + BLUE + name + RESET; 
  result = is_mut() ? result + " mutable" : result;
  result = is_rune() ? result + " rune\n" : result + '\n';
  if (has_expr()) {
    indent++;
    at_last_child = true;
    result += expr->to_string();
  }
  return result;
}


const std::string DeclStmt::to_string() {
  std::string result = piping() + BOLD + MAGENTA + "DeclStmt" + RESET + '\n';
  indent++;
  at_last_child = true;
  return result + decl->to_string();
}


const std::string CompoundStmt::to_string() {
  std::string result = piping() + BOLD + MAGENTA + "CompoundStmt" + RESET + '\n';
  indent++;
  at_last_child = false;
  unsigned base_indent = indent;
  place_vert[indent] = 1;

  for (std::unique_ptr<Stmt> const &stmt : stmts) {
    indent = base_indent;
    at_last_child = stmt == stmts.back();
    place_vert[indent] = !at_last_child;
    result += stmt->to_string();
  }
  return result;
}


const std::string ReturnStmt::to_string() {
  std::string result = piping() + BOLD + MAGENTA + "ReturnStmt" + RESET + '\n';
  indent++;
  return result + expr->to_string();
}


const std::string IfStmt::to_string() {
  std::string result = piping() + BOLD + MAGENTA + "IfStmt" + RESET + '\n';
  indent++;
  at_last_child = false;
  unsigned base_indent = indent;
  place_vert[indent] = 1;

  result += cond->to_string();
  if (else_body) {
    result += then_body->to_string();
    indent = base_indent;
    at_last_child = true;
    place_vert[indent] = 0;
    result += else_body->to_string();
  } else {
    at_last_child = true;
    indent = base_indent;
    place_vert[indent] = 0;
    result += then_body->to_string();
  }
  return result;
}


const std::string MatchCase::to_string() {
  std::string result = piping() + BOLD + MAGENTA + "MatchCase" + RESET + '\n';
  indent++;
  at_last_child = false;
  unsigned base_indent = indent;
  result += expr->to_string();
  indent = base_indent;
  at_last_child = true;
  result += body->to_string();
  return result;
}


const std::string MatchStmt::to_string() {
  std::string result = piping() + BOLD + MAGENTA + "MatchStmt" + RESET + '\n';
  indent++;
  at_last_child = false;
  unsigned base_indent = indent;
  place_vert[indent] = 1;
  result += expr->to_string();
  for (std::unique_ptr<MatchCase> const &match_case : cases) {
    at_last_child = match_case == cases.back();
    place_vert[indent] = !at_last_child;
    result += match_case->to_string();
    indent = base_indent;
  }
  return result;
}


const std::string UntilStmt::to_string() {
  std::string result = piping() + BOLD + MAGENTA + "UntilStmt" + RESET + '\n'; 
  indent++;
  at_last_child = false;
  result += cond->to_string();
  at_last_child = true;
  result += body->to_string();
  return result;
}


const std::string BreakStmt::to_string() {
  return piping() + BOLD + MAGENTA + "BreakStmt" + RESET + '\n';
}


const std::string ContinueStmt::to_string() {
  return piping() + BOLD + MAGENTA + "ContinueStmt" + RESET + '\n';
}


const std::string DefaultExpr::to_string() {
  return piping() + MAGENTA + "DefaultExpr" + GREEN + " '" + get_type()->to_string() + "' " + BOLD + CYAN + "_" + RESET + '\n';
}


const std::string NullExpr::to_string() {
  return piping() + MAGENTA + "NullExpr" + GREEN + " '" + get_type()->to_string() + "' " + BOLD + CYAN + "null" + RESET + '\n';
}


const std::string BooleanLiteral::to_string() {
  return piping() + MAGENTA + "BooleanLiteral" + GREEN + " '" + get_type()->to_string() + "' " + BOLD + CYAN + std::to_string(value) + RESET + '\n';
}


const std::string IntegerLiteral::to_string() {
  return piping() + MAGENTA + "IntegerLiteral" + GREEN + " '" + get_type()->to_string() + "' " + BOLD + CYAN + std::to_string(value) + RESET + '\n';
}


const std::string FPLiteral::to_string() {
  return piping() + MAGENTA + "FPLiteral" + GREEN + " '" + get_type()->to_string() + "' " + BOLD + CYAN + std::to_string(value) + RESET + '\n';
}


const std::string CharLiteral::to_string() {
  return piping() + MAGENTA + "CharLiteral" + GREEN + " '" + get_type()->to_string() + "' " + BOLD + CYAN + '\'' + value + '\'' + RESET + '\n';
}


const std::string StringLiteral::to_string() {
  return piping() + MAGENTA + "StringLiteral" + GREEN + " '" + get_type()->to_string() + "' " + BOLD + CYAN + "\"" + value + "\"" + RESET + '\n';
}


const std::string BinaryExpr::to_string() {
  std::string result = get_type() ? piping() + MAGENTA + "BinaryExpr" + GREEN + " '" + get_type()->to_string() + "' " + BOLD + CYAN + binary_to_string(op) + RESET + '\n' \
    : piping() + MAGENTA + "BinaryExpr " + BOLD + BLUE + binary_to_string(op) + RESET + '\n';
  indent++;
  place_vert[indent] = 1;
  int s_indent = indent;
  result += lhs->to_string();
  at_last_child = true;
  indent = s_indent;
  place_vert[indent] = 0;
  result += rhs->to_string();
  at_last_child = false;
  return result;
}


const std::string UnaryExpr::to_string() {
  std::string result = piping() + MAGENTA + "UnaryExpr" + GREEN + " '" + get_type()->to_string() + "' " + BOLD + CYAN + unary_to_string(op) + RESET + '\n';
  indent++;
  result += expr->to_string();
  return result;
}


const std::string CallExpr::to_string() {
  std::string result = get_type() ? piping() + MAGENTA + "CallExpr" + GREEN + " '" + get_type()->to_string() + "' " + BLUE + '\'' + callee + '\'' + RESET + '\n' \
    : piping() + MAGENTA + "CallExpr " + BLUE + '\'' + callee + '\'' + RESET + '\n';
  indent++;
  at_last_child = args.empty() ? true : false;
  for (std::unique_ptr<Expr> const &arg : args) {
    at_last_child = arg == args.back();
    result += arg->to_string();
  }
  at_last_child = false;
  return result;
}


const std::string DeclRefExpr::to_string() {
  const std::string type = get_type() ? get_type()->to_string() : "void";
  std::string result = piping() + MAGENTA + "DeclRefExpr" + GREEN + " '" + type + "' " + BLUE + ident + RESET + '\n';
  at_last_child = false;
  return result;
}


const std::string InitExpr::to_string() {
  std::string result = piping() + MAGENTA + "InitExpr" + GREEN + " '" + get_type()->to_string() + '\'' + RESET + '\n';
  indent++;
  place_vert[indent] = 1;
  for (std::pair<std::string, std::unique_ptr<Expr>> const &field : fields) {
    at_last_child = field == fields.back();
    if (at_last_child) {
      place_vert[indent] = 0;
    }
    result += piping() + RED + "Field" + GREEN + " '" + field.second->get_type()->to_string() + "' " + BLUE + field.first + RESET + '\n';
    at_last_child = true;
    indent++;
    result += field.second->to_string();
    indent--;
  }
  indent = 0;
  at_last_child = false;
  return result;
}


const std::string MemberExpr::to_string() {
  std::string result = get_type() ? piping() + MAGENTA + "MemberExpr" + GREEN + " '" + get_type()->to_string() + "' " + BLUE + '\'' + get_member() + '\'' + RESET + '\n' \
    : piping() + MAGENTA + "MemberExpr " + BLUE + '\'' + get_member() + '\'' + RESET + '\n';
  indent++;
  at_last_child = true;
  result += base->to_string();
  at_last_child = false;
  return result;
}


const std::string MemberCallExpr::to_string() {
  std::string result = get_type() ? piping() + MAGENTA + "MemberCallExpr" + GREEN + " '" + get_type()->to_string() + "' " + BLUE + '\'' + callee + '\'' + RESET + '\n' \
    : piping() + MAGENTA + "MemberCallExpr " + BLUE + '\'' + callee + '\'' + RESET + '\n';
  indent++;
  at_last_child = args.empty() ? true : false;
  result += base->to_string();
  for (std::unique_ptr<Expr> const &arg : args) {
    at_last_child = arg == args.back();
    result += arg->to_string();
  }
  at_last_child = false;
  return result;
}


const std::string ThisExpr::to_string() {
  return get_type() ? piping() + MAGENTA + "ThisExpr" + GREEN + " '" + get_type()->to_string() + "' " + BOLD + CYAN + "this" + RESET + '\n' \
    : piping() + MAGENTA + "ThisExpr" + GREEN + " 'unknown' " + BOLD + CYAN + "this" + RESET + '\n';
}
