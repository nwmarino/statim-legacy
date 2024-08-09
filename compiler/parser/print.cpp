/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <memory>
#include <string>
#include <utility>

#include "../core/ast.h"

const std::string ProgAST::to_str() {
  std::string result;

  for (std::unique_ptr<FunctionAST> &def : defs) {
    result += def->to_str() + '\n';
  }

  return result;
}


const std::string FunctionAST::to_str() {
  std::string result;

  result += head->to_str() + '\n';
  result += body->to_str();

  return result;
}


const std::string PrototypeAST::to_str() {
  std::string result;

  result += "function " + name + ":\n";
  result += "  params: ";

  for (const std::pair<std::string, std::string> &arg : args) {
    result += arg.second + ' ' + arg.first;
    result += ", ";
  }

  if (!args.empty()) {
    result.pop_back();
    result.pop_back();
  }

  result += "\n  return: i32\n";

  return result;
}


const std::string CompoundStatement::to_str() {
  std::string result;

  for (std::unique_ptr<Statement> &stmt : stmts) {
    result += stmt->to_str() + '\n';
  }

  return result;
}


const std::string ReturnStatement::to_str() {
  return "return\n\t" + expr->to_str();
}


const std::string IntegerExpr::to_str() {
  return std::to_string(value) + " (int)\n";
}


const std::string FloatingPointExpr::to_str() {
  return std::to_string(value) + " (float)\n";
}


const std::string NullExpr::to_str() {
  return "null\n";
}


const std::string BoolExpr::to_str() {
  return std::to_string(value) + " (bool)\n";
}


const std::string CharExpr::to_str() {
  return std::to_string(value) + " (char)\n";
}


const std::string ByteExpr::to_str() {
  return std::to_string(value) + " (byte)\n";
}


const std::string StringExpr::to_str() {
  return value + " (str)\n";
}


const std::string ByteStringExpr::to_str() {
  return value + " (bstr)\n";
}


const std::string VariableExpr::to_str() {
  return "var: " + ident + '\n';
}


const std::string AssignmentStatement::to_str() {
  std::string result;

  result += "assign\n\tname: " + ident + "\n\ttype: " + ty + '\n';
  if (expr) {
    result += "\trval: " + expr->to_str();
  }
  return result;
}


const std::string BinaryExpr::to_str() {
  return "(binop) -> " + std::to_string(op) + " : " + left_child->to_str() + " , " + right_child->to_str() + '\n';
}


const std::string FunctionCallExpr::to_str() {
  std::string result;

  result += "CALL -> " + callee + " ( ";

  for (std::unique_ptr<Expr> &arg : args) {
    result += arg->to_str();
  }

  result += " )\n";

  return result;
}
