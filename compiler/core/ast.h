/// Copyright 2024 Nick Marino (github.com/nwmarino)

#ifndef STATIMC_AST_H
#define STATIMC_AST_H

#include <string>
#include <vector>
#include <memory>
#include <utility>

/// An enumeration of all possible function return types.
typedef
enum {
  RT_VOID,
  RT_INT,
  RT_FLOAT,
  RT_BOOL,
  RT_CHAR,
  RT_STRING
} RetType;


/// An abstract syntax tree node.
class AST {
  public:
    virtual ~AST() = default;
};

/// An expression.
class Expr : public AST {
  public:
    virtual ~Expr() = default;
};

/// A statement.
class Statement : public AST {
  public:
    virtual ~Statement() = default;
};

/// Compound statements.
class CompoundStatement : public Statement {
  std::vector<std::unique_ptr<Statement>> stmts;

  public:
    CompoundStatement(std::vector<std::unique_ptr<Statement>> stmts) : stmts(std::move(stmts)) {};
};

/// Integer literal expressions.
class IntegerExpr : public Expr {
  int value;

  public:
    IntegerExpr(int value) : value(value) {};
};

/// Floating point literal expressions.
class FloatingPointExpr : public Expr {
  double value;

  public:
    FloatingPointExpr(double value) : value(value) {};
};

/// Variable expressions.
class VariableExpr : public Expr {
  std::string ident;

  public:
    VariableExpr(const std::string &ident) : ident(ident) {};
};

/// Binary operation expressions.
class BinaryExpr : public Expr {
  char op;
  std::unique_ptr<Expr> left_child, right_child;

  public:
    BinaryExpr(char op, std::unique_ptr<Expr> left_child, std::unique_ptr<Expr> right_child)
      : op(op), left_child(std::move(left_child)), right_child(std::move(right_child)) {};
};

/// Function call expressions;
class FunctionCallExpr : public Expr {
  std::string callee;
  std::vector<std::unique_ptr<Expr>> args;

  public:
    FunctionCallExpr(const std::string &callee, std::vector<std::unique_ptr<Expr>> args)
      : callee(callee), args(std::move(args)) {};
};

/// Functions prototypes.
class PrototypeAST : public AST {
  std::string name;
  std::vector<std::string> args;

  public:
    PrototypeAST(const std::string &name, std::vector<std::string> args)
      : name(name), args(std::move(args)) {};
};

/// Function definitions.
class FunctionAST : public AST {
  std::unique_ptr<PrototypeAST> head;
  std::unique_ptr<Statement> body;

  public:
    FunctionAST(std::unique_ptr<PrototypeAST> head, std::unique_ptr<Statement> body)
      : head(std::move(head)), body(std::move(body)) {};
};

/// Return statements.
class ReturnStatement : public Statement {
  std::unique_ptr<Expr> expr;

  public:
    ReturnStatement(std::unique_ptr<Expr> expr) : expr(std::move(expr)) {};
};

#endif  // STATIMC_AST_H
