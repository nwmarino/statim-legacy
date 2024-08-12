/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <memory>
#include <string>
#include <utility>

#include "../core/ast.h"
#include "../core/token.h"

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
  std::string result;

  result.append(n, ' ') += "return\n";
  if (expr) {
    result.append(n + 2, ' ') += "expr: " + expr->to_str(0);
  }
  return result;
}


const std::string IntegerExpr::to_str(int n) {
  return std::string().append(n, ' ') += std::to_string(value) + " (int)";
}


const std::string FloatingPointExpr::to_str(int n) {
  return std::string().append(n, ' ') += std::to_string(value) + " (float)";
}


const std::string NullExpr::to_str(int n) {
  return std::string().append(n, ' ') += "null";
}


const std::string BoolExpr::to_str(int n) {
  return std::string().append(n, ' ') += std::to_string(value) + " (bool)";
}


const std::string CharExpr::to_str(int n) {
  return std::string().append(n, ' ') += std::to_string(value) + " (char)";
}


const std::string ByteExpr::to_str(int n) {
  return std::string().append(n, ' ') += std::to_string(value) + " (byte)";
}


const std::string StringExpr::to_str(int n) {
  return std::string().append(n, ' ') + value + " (str)";
}


const std::string ByteStringExpr::to_str(int n) {
  return std::string().append(n, ' ') + value + " (bstr)";
}


const std::string VariableExpr::to_str(int n) {
  return std::string().append(n, ' ') + ident + " (var)";
}


const std::string AssignmentStatement::to_str(int n) {
  std::string result;

  result.append(n, ' ') += "assign\n";
  result.append(n + 2, ' ') += "name: " + ident + '\n';
  result.append(n + 2, ' ') += "type: " + ty + '\n';
  if (expr) {
    result.append(n + 2, ' ') += "rval: " + expr->to_str(0) + '\n';
  }
  return result;
}


const std::string BinaryExpr::to_str(int n) {
  std::string result;
  std::string optr;

  switch (op) {
    case TokenKind::Add: optr = "+"; break;
    case TokenKind::Sub: optr = "-"; break;
    case TokenKind::Star: optr = "*"; break;
    case TokenKind::Slash: optr = "/"; break;
    case TokenKind::Eq: optr = "="; break;
  }

  result.append(n, ' ') += "binop\n";
  result.append(n + 2, ' ') += "op: " + optr + '\n';
  result += left_child->to_str(n + 4) + '\n';
  result += right_child->to_str(n + 4) + '\n';
  result.pop_back();
  return result;
}


const std::string FunctionCallExpr::to_str(int n) {
  std::string result;

  result.append(n, ' ') += "call @" + callee + '(';

  for (std::unique_ptr<Expr> &arg : args) {
    result += arg->to_str(0) + ", ";
  }

  if (!args.empty()) {
    result.pop_back();
    result.pop_back();
  }

  result += ')';

  return result;
}


const std::string IfStatement::to_str(int n) {
  std::string result;

  result.append(n, ' ') += "if\n";
  result.append(n + 2, ' ') += "cond: " + cond->to_str(0) + '\n';

  result += then_body->to_str(n + 2);

  if (else_body) {
    result += '\n';
    result.append(n, ' ') += "else\n";
    result += else_body->to_str(n + 2);
  }

  return result;
}


const std::string UntilStatement::to_str(int n) {
  std::string result;

  result.append(n, ' ') += "until\n";
  result.append(n + 2, ' ') += "cond: " + cond->to_str(0) + "\n\n";

  result += body->to_str(n + 2);

  return result;
}