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

/// Return statements.
class ReturnStatement : public Statement {
  std::unique_ptr<Expr> expr;

  public:
    ReturnStatement(std::unique_ptr<Expr> expr) : expr(std::move(expr)) {};
};

/// If statements.
class IfStatement : public Statement {
  std::unique_ptr<Expr> cond;
  std::unique_ptr<Statement> then_b, else_b;

  public:
    IfStatement(std::unique_ptr<Expr> cond, std::unique_ptr<Statement> then_b, std::unique_ptr<Statement> else_b)
      : cond(std::move(cond)), then_b(std::move(then_b)), else_b(std::move(else_b)) {};
};

/// While statements.
class WhileStatement : public Statement {
  std::unique_ptr<Expr> cond;
  std::unique_ptr<Statement> body;

  public:
    WhileStatement(std::unique_ptr<Expr> cond, std::unique_ptr<Statement> body)
      : cond(std::move(cond)), body(std::move(body)) {};
};

/// Break statements.
class BreakStatement : public Statement {
  public:
    BreakStatement() {};
};

/// Match statements.
class MatchStatement : public Statement {
  std::unique_ptr<Expr> expr;
  std::vector<std::pair<std::unique_ptr<Expr>, std::unique_ptr<Statement>>> cases;

  public:
    MatchStatement(std::unique_ptr<Expr> expr, std::vector<std::pair<std::unique_ptr<Expr>, std::unique_ptr<Statement>>> cases)
      : expr(std::move(expr)), cases(std::move(cases)) {};
};

/// Variable expression.
///
/// `x`, `y`, `z`
class VariableExpr : public Expr {
  const std::string ident;

  public:
    VariableExpr(const std::string &ident) : ident(ident) {};
};

/// Null literal expressions.
///
/// `null`
class NullExpr : public Expr {
  public:
    NullExpr() {};
};

/// Boolean literal expressions.
///
/// `true`, `false`
class BoolExpr : public Expr {
  const bool value;

  public:
    BoolExpr(bool value) : value(value) {};
};

/// Integer literal expressions.
///
/// @example `0`, `512`, `1024`
class IntegerExpr : public Expr {
  const int value;

  public:
    IntegerExpr(int value) : value(value) {};
};

/// Floating point literal expressions.
///
/// @example `0.0`, `3.14`, `6.28`
class FloatingPointExpr : public Expr {
  const double value;

  public:
    FloatingPointExpr(double value) : value(value) {};
};

/// Character literal expressions.
///
/// @example `'a'`, `'b'`, `'c'`
class CharExpr : public Expr {
  const char value;

  public:
    CharExpr(char value) : value(value) {};
};

/// String literal expressions.
///
/// @example `"hello, world"`, `"foo"`, `"bar"`
class StringExpr : public Expr {
  const std::string value;

  public:
    StringExpr(const std::string &value) : value(value) {};
};

/// Byte literal expressions.
///
/// @example `b'a'`, `b'b'`, `b'c'`
class ByteExpr : public Expr {
  const char value;

  public:
    ByteExpr(char value) : value(value) {};
};

/// Byte string literal expressions.
///
/// @example `b"hello, world"`, `b"foo"`, `b"bar"`
class ByteStringExpr : public Expr {
  const std::string value;

  public:
    ByteStringExpr(const std::string &value) : value(value) {};
};

/// Binary operation expressions.
///
/// @example `x + y`, `1 - y`, `x * 2`
class BinaryExpr : public Expr {
  char op;
  std::unique_ptr<Expr> left_child, right_child;

  public:
    BinaryExpr(char op, std::unique_ptr<Expr> left_child, std::unique_ptr<Expr> right_child)
      : op(op), left_child(std::move(left_child)), right_child(std::move(right_child)) {};
};

/// Function call expressions;
///
/// @example `foo()`, `bar(x, y, 3)`
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

#endif  // STATIMC_AST_H
