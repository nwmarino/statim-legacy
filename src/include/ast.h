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


/**
 * Base class to all other abstract syntax nodes.
 */
class AST {
  public:
    virtual ~AST() = default;
};


/**
 * Base class to expression nodes.
 */
class Expr : public AST {
  public:
    virtual ~Expr() = default;
};


/**
 * Base class to statement nodes.
 */
class Statement : public AST {
  public:
    virtual ~Statement() = default;
};


/**
 * Class for compound statements.
 */
class CompoundStatement : public Statement {
  std::vector<std::unique_ptr<Statement>> stmts;

  public:
    CompoundStatement(std::vector<std::unique_ptr<Statement>> stmts)
      : stmts(std::move(stmts)) {}
};


/**
 * Expression class for integer literals.
 */
class IntegerExpr : public Expr {
  long long value;

  public:
    IntegerExpr(long long value) : value(value) {}
};


/**
 * Expression class for floating point literals.
 */
class FloatingPointExpr : public Expr {
  double value;

  public:
    FloatingPointExpr(double value) : value(value) {}
};


/**
 * Expression class for variables.
 */
class VariableExpr : public Expr {
  std::string name;

  public:
    VariableExpr(const std::string &name) : name(name) {}
};


/**
 * Expression class for assignments.
 */
class AssignStatement : public Statement {
  std::string name;
  std::unique_ptr<Expr> value;

  public:
    AssignStatement(const std::string &name, std::unique_ptr<Expr> value)
      : name(name), value(std::move(value)) {}
};


/**
 * Expression class for binary operations.
 */
class BinaryExpr : public Expr {
  char op;
  std::unique_ptr<Expr> leftSide, rightSide;

  public:
    BinaryExpr(char op, std::unique_ptr<Expr> leftSide, std::unique_ptr<Expr> rightSide)
      : op(op), leftSide(std::move(leftSide)), rightSide(std::move(rightSide)) {}
};


/**
 * Expression class for function calls.
 */
class FunctionCallExpr : public Expr {
  std::string callee;
  std::vector<std::unique_ptr<Expr>> args;

  public:
    FunctionCallExpr(const std::string &callee, std::vector<std::unique_ptr<Expr>> args)
      : callee(callee), args(std::move(args)) {}
};


/**
 * Node class for function prototypes.
 */
class PrototypeAST : public AST {
  std::string name;
  std::vector<std::string> args;
  RetType retType;

  public:
    PrototypeAST(const std::string &name, std::vector<std::string> args, RetType retType)
      : name(name), args(std::move(args)), retType(retType) {}
    std::string getName() { return name; }
    RetType getRetType() { return retType; }
};


/**
 * Node class for function definitions.
 */
class FunctionAST : public AST {
  std::unique_ptr<PrototypeAST> head;
  std::unique_ptr<Statement> body;

  public:
    FunctionAST(std::unique_ptr<PrototypeAST> head, std::unique_ptr<Statement> body)
      : head(std::move(head)), body(std::move(body)) {}
};


/**
 * Statement class for return statements.
 */
class ReturnStatement : public Statement {
  std::unique_ptr<Expr> expr;

  public:
    ReturnStatement(std::unique_ptr<Expr> expr) : expr(std::move(expr)) {}
};


#endif  // STATIMC_AST_H
