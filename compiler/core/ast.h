/// Copyright 2024 Nick Marino (github.com/nwmarino)

#ifndef STATIMC_AST_H
#define STATIMC_AST_H

#include "token.h"
#include <string>
#include <vector>
#include <memory>
#include <utility>

/// An abstract syntax tree node.
class AST {
  public:
    virtual ~AST() = default;
    const virtual std::string to_str(int n) = 0;
};

/// A statement.
class Statement : public AST {
  public:
    virtual ~Statement() = default;
    const virtual std::string to_str(int n) = 0;
};

/// An expression (or statement ending with semi).
class Expr : public Statement {
  public:
    virtual ~Expr() = default;
    const virtual std::string to_str(int n) = 0;
};

/// Compound statements.
class CompoundStatement : public Statement {
  std::vector<std::unique_ptr<Statement>> stmts;

  public:
    CompoundStatement(std::vector<std::unique_ptr<Statement>> stmts) : stmts(std::move(stmts)) {};
    const std::string to_str(int n);
};

/// Return statements.
class ReturnStatement : public Statement {
  std::unique_ptr<Expr> expr;

  public:
    ReturnStatement(std::unique_ptr<Expr> expr) : expr(std::move(expr)) {};
    const std::string to_str(int n);
};

/// Variable declaration.
///
/// `fix x: int = 0`, `let y: int = 1`
class AssignmentStatement : public Statement {
  const std::string ident;
  const std::string ty;
  std::unique_ptr<Expr> expr;

  public:
    /// Constructor for reassignment.
    AssignmentStatement(const std::string &ident, std::unique_ptr<Expr> expr)
      : ident(ident), ty(""), expr(std::move(expr)) {};
    /// Constructor for initial assignment.
    AssignmentStatement(const std::string &ident, const std::string &ty, std::unique_ptr<Expr> expr)
      : ident(ident), ty(ty), expr(std::move(expr)) {};
    const std::string to_str(int n);
};

/// If statement
class IfStatement : public Statement {
  std::unique_ptr<Expr> cond;
  std::unique_ptr<Statement> then_body;
  std::unique_ptr<Statement> else_body;

  public:
    IfStatement(std::unique_ptr<Expr> cond, std::unique_ptr<Statement> then_body, std::unique_ptr<Statement> else_body)
      : cond(std::move(cond)), then_body(std::move(then_body)), else_body(std::move(else_body)) {};
    const std::string to_str(int n);
};

/// Until loop statements.
class UntilStatement : public Statement { 
  std::unique_ptr<Expr> cond;
  std::unique_ptr<Statement> body;

  public:
    UntilStatement(std::unique_ptr<Expr> cond, std::unique_ptr<Statement> body)
      : cond(std::move(cond)), body(std::move(body)) {};
    const std::string to_str(int n);
};

/// Match case.
class MatchCase : public AST {
  std::unique_ptr<Expr> expr;
  std::unique_ptr<Statement> body;

  public:
    MatchCase(std::unique_ptr<Expr> expr, std::unique_ptr<Statement> body)
      : expr(std::move(expr)), body(std::move(body)) {};
    const std::string to_str(int n);
};

/// Match statement.
class MatchStatement : public Statement {
  std::unique_ptr<Expr> expr;
  std::vector<std::unique_ptr<MatchCase>> cases;

  public:
    MatchStatement(std::unique_ptr<Expr> expr, std::vector<std::unique_ptr<MatchCase>> cases)
      : expr(std::move(expr)), cases(std::move(cases)) {};
    const std::string to_str(int n);
};

/// Default expression.
class DefaultExpr : public Expr {
  public:
    DefaultExpr() {};
    const std::string to_str(int n);
};

/// Variable expression.
///
/// `x`, `y`, `z`
class VariableExpr : public Expr {
  const std::string ident;

  public:
    VariableExpr(const std::string &ident) : ident(ident) {};
    const std::string to_str(int n);
};

/// Null literal expressions.
///
/// `null`
class NullExpr : public Expr {
  public:
    NullExpr() {};
    const std::string to_str(int n);
};

/// Boolean literal expressions.
///
/// `true`, `false`
class BoolExpr : public Expr {
  bool value;

  public:
    BoolExpr(bool value) : value(value) {};
    const std::string to_str(int n);
};

/// Integer literal expressions.
///
/// @example `0`, `512`, `1024`
class IntegerExpr : public Expr {
  const int value;

  public:
    IntegerExpr(int value) : value(value) {};
    const std::string to_str(int n);
};

/// Floating point literal expressions.
///
/// @example `0.0`, `3.14`, `6.28`
class FloatingPointExpr : public Expr {
  const double value;

  public:
    FloatingPointExpr(double value) : value(value) {};
    const std::string to_str(int n);
};

/// Character literal expressions.
///
/// @example `'a'`, `'b'`, `'c'`
class CharExpr : public Expr {
  const char value;

  public:
    CharExpr(char value) : value(value) {};
    const std::string to_str(int n);
};

/// String literal expressions.
///
/// @example `"hello, world"`, `"foo"`, `"bar"`
class StringExpr : public Expr {
  const std::string value;

  public:
    StringExpr(const std::string &value) : value(value) {};
    const std::string to_str(int n);
};

/// Byte literal expressions.
///
/// @example `b'a'`, `b'b'`, `b'c'`
class ByteExpr : public Expr {
  const char value;

  public:
    ByteExpr(char value) : value(value) {};
    const std::string to_str(int n);
};

/// Byte string literal expressions.
///
/// @example `b"hello, world"`, `b"foo"`, `b"bar"`
class ByteStringExpr : public Expr {
  const std::string value;

  public:
    ByteStringExpr(const std::string &value) : value(value) {};
    const std::string to_str(int n);
};

/// Binary operation expressions.
///
/// @example `x + y`, `1 - y`, `x * 2`
class BinaryExpr : public Expr {
  TokenKind op;
  std::unique_ptr<Expr> left_child, right_child;

  public:
    BinaryExpr(TokenKind op, std::unique_ptr<Expr> left_child, std::unique_ptr<Expr> right_child)
      : op(op), left_child(std::move(left_child)), right_child(std::move(right_child)) {};
    const std::string to_str(int n);
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
    const std::string to_str(int n);
};

/// Functions prototypes.
class PrototypeAST : public AST {
  std::string name;
  std::vector<std::pair<std::string, std::string>> args;
  std::string ret_ty;

  public:
    PrototypeAST(const std::string &name, std::vector<std::pair<std::string, std::string>> args, const std::string &ret_ty)
      : name(name), args(std::move(args)), ret_ty(ret_ty) {};
    const std::string to_str(int n);
    const std::string get_name() { return name; }
};

/// Function definitions.
class FunctionAST : public AST {
  std::unique_ptr<PrototypeAST> head;
  std::unique_ptr<Statement> body;

  public:
    FunctionAST(std::unique_ptr<PrototypeAST> head, std::unique_ptr<Statement> body)
      : head(std::move(head)), body(std::move(body)) {};
    const std::string to_str(int n);
    const std::string get_name() { return head->get_name(); }
};

/// Abstract interface for a struct.
class AbstractAST : public AST {
  std::string name;
  std::vector<std::unique_ptr<PrototypeAST>> decls;

  public:
    AbstractAST(const std::string &name, std::vector<std::unique_ptr<PrototypeAST>> decls)
      : name(name), decls(std::move(decls)) {};
    const std::string to_str(int n);
};

/// Struct definitions.
class StructAST : public AST {
  std::string name;
  std::vector<std::pair<std::string, std::string>> fields;

  public:
    StructAST(const std::string &name, std::vector<std::pair<std::string, std::string>> fields)
      : name(name), fields(std::move(fields)) {};
    const std::string to_str(int n);
};

/// Enum definitions.
class EnumAST : public AST {
  std::string name;
  std::vector<std::string> variants;

  public:
    EnumAST(const std::string &name, std::vector<std::string> variants)
      : name(name), variants(std::move(variants)) {};
    const std::string to_str(int n);
};

/// Implementation of a struct.
class ImplAST : public AST {
  std::string struct_name;
  std::string abstract;
  std::vector<std::unique_ptr<FunctionAST>> defs;

  public:
    ImplAST(const std::string &struct_name, const std::string &abstract, std::vector<std::unique_ptr<FunctionAST>> defs)
      : struct_name(struct_name), abstract(abstract), defs(std::move(defs)) {};
    const std::string to_str(int n);
};

/// A package (list of definitions).
class PackageAST : public AST {
  std::string name;
  std::vector<std::unique_ptr<AST>> defs;
  std::vector<std::string> imports;

  public:
    PackageAST(const std::string &name, std::vector<std::unique_ptr<AST>> defs, std::vector<std::string> imports)
      : name(name), defs(std::move(defs)), imports(std::move(imports)) {};
    const std::string to_str(int n);
};

/// A program (list of packages).
class ProgAST : public AST {
  std::vector<std::unique_ptr<PackageAST>> pkgs;

  public:
    ProgAST(std::vector<std::unique_ptr<PackageAST>> pkgs) : pkgs(std::move(pkgs)) {};
    const std::string to_str(int n);
};

#endif  // STATIMC_AST_H
