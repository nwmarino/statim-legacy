/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <memory>

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

  result += head->to_str();
  result += "\n body:\n\n";
  result += body->to_str();

  return result;
}

const std::string PrototypeAST::to_str() {
  std::string result;

  result += name;
  result += "\n args: ";

  for (const std::string &arg : args) {
    result += arg;
    result += ", ";
  }

  if (!args.empty()) {
    result.pop_back();
    result.pop_back();
  }

  result += "\n ret: 'void'";

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
  return "RETURN\n\t" + expr->to_str() + '\n';
}

const std::string IntegerExpr::to_str() {
  return "INTEGER -> " + std::to_string(value) + '\n';
}

const std::string FloatingPointExpr::to_str() {
  return "FLOAT -> " + std::to_string(value) + '\n';
}

const std::string NullExpr::to_str() {
  return "NULL\n";
}

const std::string BoolExpr::to_str() {
  return "BOOL -> " + std::to_string(value) + '\n';
}

const std::string CharExpr::to_str() {
  return "CHAR -> " + std::to_string(value) + '\n';
}

const std::string ByteExpr::to_str() {
  return "BYTE : " + std::to_string(value) + '\n';
}

const std::string StringExpr::to_str() {
  return "STRING -> " + value + '\n';
}

const std::string ByteStringExpr::to_str() {
  return "BYTESTRING -> " + value + '\n';
}

const std::string VariableExpr::to_str() {
  return "VARIABLE -> " + ident + '\n';
}

const std::string AssignmentStatement::to_str() {
  return "ASSIGNMENT -> " + ident + " : " + ty + " = " + expr->to_str() + '\n';
}

const std::string BinaryExpr::to_str() {
  return "BINARY -> " + std::to_string(op) + " : " + left_child->to_str() + " , " + right_child->to_str() + '\n';
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
