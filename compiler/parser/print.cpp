/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <memory>
#include <string>
#include <utility>

#include "../core/ast.h"

const std::string ProgAST::to_str(int n) {
  std::string result;

  for (std::unique_ptr<FunctionAST> &def : defs) {
    result += def->to_str(0);
  }

  return result;
}


const std::string FunctionAST::to_str(int n) {
  std::string result;

  result += head->to_str(0) + '\n';
  result += body->to_str(2);

  return result;
}


const std::string PrototypeAST::to_str(int n) {
  std::string result;

  result.append(n, ' ') += "function " + name + '\n';
  result.append(n + 2, ' ') += "params: ";

  for (const std::pair<std::string, std::string> &arg : args) {
    result += arg.first + ": " + arg.second + ", ";
  }

  if (!args.empty()) {
    result.pop_back();
    result.pop_back();
  }

  result += '\n';

  result.append(n + 2, ' ') += "return: i32\n";

  return result;
}


const std::string CompoundStatement::to_str(int n) {
  std::string result;

  for (std::unique_ptr<Statement> &stmt : stmts) {
    result += stmt->to_str(n) + '\n';
  }

  return result;
}


const std::string ReturnStatement::to_str(int n) {
  return std::string().append(n, ' ') += "return\n\t" + expr->to_str(0);
}


const std::string IntegerExpr::to_str(int n) {
  return std::string().append(n, ' ') += std::to_string(value) + " (int)\n";
}


const std::string FloatingPointExpr::to_str(int n) {
  return std::string().append(n, ' ') += std::to_string(value) + " (float)\n";
}


const std::string NullExpr::to_str(int n) {
  return std::string().append(n, ' ') += "null\n";
}


const std::string BoolExpr::to_str(int n) {
  return std::string().append(n, ' ') += std::to_string(value) + " (bool)\n";
}


const std::string CharExpr::to_str(int n) {
  return std::string().append(n, ' ') += std::to_string(value) + " (char)\n";
}


const std::string ByteExpr::to_str(int n) {
  return std::string().append(n, ' ') += std::to_string(value) + " (byte)\n";
}


const std::string StringExpr::to_str(int n) {
  return std::string().append(n, ' ') + value + " (str)\n";
}


const std::string ByteStringExpr::to_str(int n) {
  return std::string().append(n, ' ') + value + " (bstr)\n";
}


const std::string VariableExpr::to_str(int n) {
  return std::string().append(n, ' ') + "var: " + ident + '\n';
}


const std::string AssignmentStatement::to_str(int n) {
  std::string result;

  result.append(n, ' ') += "assign\n";
  result.append(n + 2, ' ') += "name: " + ident + '\n';
  result.append(n + 2, ' ') += "type: " + ty + '\n';
  if (expr) {
    result.append(n + 2, ' ') += "rval: " + expr->to_str(0);
  }
  return result;
}


const std::string BinaryExpr::to_str(int n) {
  return "(binop) -> " + std::to_string(op) + " : " + left_child->to_str(0) + " , " + right_child->to_str(0) + '\n';
}


const std::string FunctionCallExpr::to_str(int n) {
  std::string result;

  result.append(n, ' ') += "call @" + callee + '\n';

  for (std::unique_ptr<Expr> &arg : args) {
    result.append(n + 2, ' ') += arg->to_str(0);
  }

  return result;
}
