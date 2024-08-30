#include "../include/ast/Decl.h"
#include "../include/ast/Expr.h"
#include "../include/ast/Stmt.h"
#include "../include/ast/Unit.h"

/// Returns a string representation of piping dependent on an indentation `n`.
static const std::string mk_piping(int n ) {
  std::string result;
  for (int i = 0; i < n; i += 2) {
    result += '|' + std::string(2, ' ');
  }
  return result;
}


/// Returns the string representation of a unary operator.
static std::string unary_to_string(UnaryOp op) {
  switch (op) {
    case UnaryOp::Bang:    return "!";
    case UnaryOp::Rune:    return "#";
    case UnaryOp::Ref:     return "@";
    case UnaryOp::Ellipse: return "...";
    case UnaryOp::Access:  return ".";
    default:               return "";
  }
  return "";
}


/// Returns the string representation of a binary operator.
static std::string binary_to_string(BinaryOp op) {
  switch (op) {
    case BinaryOp::Assign:       return "=";
    case BinaryOp::AddAssign:    return "+=";
    case BinaryOp::SubAssign:    return "-=";
    case BinaryOp::StarAssign:   return "*=";
    case BinaryOp::SlashAssign:  return "/=";
    case BinaryOp::IsEq:         return "==";
    case BinaryOp::IsNotEq:      return "!=";
    case BinaryOp::LogicAnd:     return "&&";
    case BinaryOp::LogicOr:      return "||";
    case BinaryOp::LogicXor:     return "^^";
    case BinaryOp::BitAnd:       return "&";
    case BinaryOp::BitOr:        return "|";
    case BinaryOp::BitXor:       return "^";
    case BinaryOp::Lt:           return "<";
    case BinaryOp::LtEquals:     return "<=";
    case BinaryOp::Gt:           return ">";
    case BinaryOp::GtEquals:     return ">=";
    case BinaryOp::BitLeftShift: return "<<";
    case BinaryOp::BitRightShift:return ">>";
    case BinaryOp::Plus:         return "+";
    case BinaryOp::Minus:        return "-";
    case BinaryOp::Mult:         return "*";
    case BinaryOp::Div:          return "/";
  }
  return "";
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
  std::string result = is_priv() ? mk_piping(n) + "FunctionDecl " + name + " 'private'\n" : mk_piping(n) + "FunctionDecl " + name + '\n';
  for (std::unique_ptr<ParamVarDecl> &param : params) {
    result += param->to_string(n + 2);
  }
  if (has_body()) {
    result += body->to_string(n + 2);
  }
  return result;
}


const std::string ParamVarDecl::to_string(int n) {
  return mk_piping(n) + "ParamVarDecl " + name + '\n';
}


const std::string FieldDecl::to_string(int n) {
  return is_priv() ? mk_piping(n) + "FieldDecl " + name + " 'private'\n" : mk_piping(n) + "FieldDecl " + name + '\n';
}


const std::string StructDecl::to_string(int n) {
  std::string result = is_priv() ? mk_piping(n) + "StructDecl " + name + " 'private'\n" : mk_piping(n) + "StructDecl " + name + '\n';
  for (std::unique_ptr<FieldDecl> const &field : fields) {
    result += field->to_string(n + 2);
  }
  return result;
}


const std::string ImplDecl::to_string(int n) {
  std::string result = mk_piping(n) + "ImplDecl " + get_name();
  if (trait() != "") {
    result += " '" + trait() + "'";
  }
  result += '\n';
  for (std::unique_ptr<FunctionDecl> const &method : methods) {
    result += method->to_string(n + 2);
  }
  return result;
}


const std::string EnumVariant::to_string(int n) {
  return mk_piping(n) + "EnumVariant " + name + '\n';
}


const std::string EnumDecl::to_string(int n) {
  std::string result = is_priv() ? mk_piping(n) + "EnumDecl " + name + " 'private'\n" : mk_piping(n) + "EnumDecl " + name + '\n';
  for (EnumVariant &variant : variants) {
    result += variant.to_string(n + 2);
  }
  return result;
}


const std::string TraitDecl::to_string(int n) {
  std::string result = is_priv() ? mk_piping(n) + "TraitDecl " + name + " 'private'\n" : mk_piping(n) + "TraitDecl " + name + '\n';
  for (std::unique_ptr<FunctionDecl> &decl : decls) {
    result += decl->to_string(n + 2);
  }
  return result;
}


const std::string VarDecl::to_string(int n) {
  std::string result = mk_piping(n) + "VarDecl " + name;
  if (is_mut()) {
    result += " 'mut'";
  }
  if (is_rune()) {
    result += " 'rune'";
  }
  result += '\n';
  if (has_expr()) {
    result += expr->to_string(n + 2);
  }
  return result;
}


const std::string DeclStmt::to_string(int n) {
  return mk_piping(n) + "DeclStmt\n" + decl->to_string(n + 2);
}


const std::string CompoundStmt::to_string(int n) {
  std::string result;
  result += mk_piping(n) + "CompoundStmt\n";
  for (std::unique_ptr<Stmt> const &stmt : stmts) {
    result += stmt->to_string(n + 2);
  }
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


const std::string BooleanLiteral::to_string(int n) {
  return mk_piping(n) + "BooleanExpr " + std::to_string(value) + '\n';
}


const std::string IntegerLiteral::to_string(int n) {
  return mk_piping(n) + "IntegerExpr " + std::to_string(value) + '\n';
}


const std::string FPLiteral::to_string(int n) {
  return mk_piping(n) + "FloatingPointExpr " + std::to_string(value) + '\n';
}


const std::string CharLiteral::to_string(int n) {
  return mk_piping(n) + "CharExpr " + std::to_string(value) + '\n';
}


const std::string StringLiteral::to_string(int n) {
  return mk_piping(n) + "StringExpr " + value + '\n';
}


const std::string BinaryExpr::to_string(int n) {
  std::string result = mk_piping(n) + "BinaryExpr " + binary_to_string(op) + '\n';
  result += lhs->to_string(n + 2);
  result += rhs->to_string(n + 2);
  return result;
}


const std::string UnaryExpr::to_string(int n) {
  std::string result = mk_piping(n) + "UnaryExpr " + unary_to_string(op) + '\n';
  result += expr->to_string(n + 2);
  return result;
}


const std::string CallExpr::to_string(int n) {
  std::string result = mk_piping(n) + "CallExpr " + callee + '\n';
  for (std::unique_ptr<Expr> const &arg : args) {
    result += arg->to_string(n + 2);
  }
  return result;
}


const std::string VarExpr::to_string(int n) {
  return mk_piping(n) + "VariableExpr " + ident + '\n';
}


const std::string InitExpr::to_string(int n) {
  std::string result = mk_piping(n) + "InitExpr " + ident + '\n';
  for (std::pair<std::string, std::unique_ptr<Expr>> const &field : fields) {
    result += mk_piping(n + 2) + "Field " + field.first + '\n';
    result += field.second->to_string(n + 4);
  }
  return result;
}


const std::string MemberExpr::to_string(int n) {
  std::string result = mk_piping(n) + "MemberExpr " + member + '\n';
  result += base->to_string(n + 2);
  return result;
}


const std::string MemberCallExpr::to_string(int n) {
  std::string result = mk_piping(n) + "MemberCallExpr " + callee + '\n';
  result += base->to_string(n + 2);
  for (std::unique_ptr<Expr> const &arg : args) {
    result += arg->to_string(n + 2);
  }
  return result;
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
  
  for (Decl *d : decls) {
    // if the declaration is a scopeddecl, stringify its scope
    if (ScopedDecl *sd = dynamic_cast<ScopedDecl *>(d)) {
      result += sd->get_scope()->to_string(n + 2);
    }
    result += d->to_string(n + 2);
  }
  return result;
}
