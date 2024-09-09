#ifndef EXPR_STATIMC_H
#define EXPR_STATIMC_H

/// Expression AST nodes.
/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include "Stmt.h"
#include "../core/Type.h"
#include "../sema/ASTVisitor.h"

/// UnaryOp - Enumeration of recognized unary operators.
typedef enum {
  /// `!`
  Bang,

  /// `#`
  Rune,

  /// `@`
  Ref,

  /// `.`
  Access,

  UnknownUnaryOp,
} UnaryOp;


/// BinaryOp - Enumeration of recognized binary operators.
typedef enum {
  /// `=`
  Assign,

  /// `+=`
  AddAssign,

  /// `-=`
  SubAssign,

  /// `*=`
  SlashAssign,

  /// `/=`
  StarAssign,

  /// `==`
  IsEq,

  /// `!=`
  IsNotEq,

  /// `&&`
  LogicAnd,

  /// `||`
  LogicOr,

  /// `<`
  Lt,

  /// `<=`
  LtEquals,

  /// `>`
  Gt,

  /// `>=`
  GtEquals,

  /// `+`
  Plus,

  /// `-`
  Minus,

  /// `*`
  Mult,

  /// `/`
  Div,

  UnknownBinaryOp,
} BinaryOp;


/// Base class for expressions; statements that may have a value and type.
class Expr : public Stmt
{
public:
  virtual ~Expr() = default;
  virtual void pass(ASTVisitor *visitor) = 0;
  virtual const Type* get_type() const = 0;
  virtual const std::string to_string() = 0;
};


/// NullExpr - Represents a null expression.
///
/// @example `null`
class NullExpr final : public Expr
{
private:
  const Type *T;

public:
  NullExpr(Type *T) : T(T){};
  void pass(ASTVisitor *visitor) override { visitor->visit(this); }
  inline const Type* get_type() const override { return T; }

  /// Returns a string representation of this null expression.
  const std::string to_string() override;
};


/// DefaultExpr - Represents a default expression in pattern matching.
///
/// @example `_`
class DefaultExpr final : public Expr
{
private:
  const Type *T;

public:
  DefaultExpr(const Type *T) : T(T){};
  void pass(ASTVisitor *visitor) override { visitor->visit(this); }
  inline const Type* get_type() const override { return T; }

  /// Returns a string representation of this expression.
  const std::string to_string() override;
};


/// BooleanLiteral - Represents a boolean literal expression.
///
/// @example `true`, `false`
class BooleanLiteral final : public Expr
{
private:
  const unsigned int value;
  const Type *T;

public:
  BooleanLiteral(bool value, const Type *T) : value(value ? 1 : 0), T(T){};
  void pass(ASTVisitor *visitor) override { visitor->visit(this); }
  inline const Type* get_type() const override { return T; }

  /// Gets the value of this boolean expression.
  inline bool get_value() const { return value; }

  /// Returns a string representation of this boolean expression.
  const std::string to_string() override;
};


/// IntegerLiteral - Represents an integer literal expression.
///
/// @example `0`, `512`, `1024`
class IntegerLiteral final : public Expr
{
private:
  const long value;
  const bool signedness;
  const Type *T;

public:
  IntegerLiteral(int value, const Type *T) : value(value), signedness(value < 0), T(T){};
  void pass(ASTVisitor *visitor) override { visitor->visit(this); }
  inline const Type* get_type() const override { return T; }

  /// Gets the value of this integer expression.
  inline int get_value() const { return value; }

  /// Gets the signedness of this integer expression.
  inline bool is_signed() const { return signedness; }

  /// Returns a string representation of this integer expression.
  const std::string to_string() override;
};


/// FPLiteral - Represents a floating point literal expression.
///
/// @example `0.0`, `3.14`, `6.28`
class FPLiteral final : public Expr
{
private:
  const double value;
  const Type *T;

public:
  FPLiteral(double value, const Type *T) : value(value), T(T) {};
  void pass(ASTVisitor *visitor) override { visitor->visit(this); }
  inline const Type* get_type() const override { return T; }

  /// Gets the value of this floating point expression.
  inline double get_value() const { return value; }

  /// Returns a string representation of this floating point expression.
  const std::string to_string() override;
};


/// CharLiteral - Represents a character literal expression.
///
/// @example `'a'`, `'b'`, `'c'`
class CharLiteral final : public Expr
{
private:
  const char value;
  const Type *T;

public:
  CharLiteral(char value, const Type *T) : value(value), T(T){};
  void pass(ASTVisitor *visitor) override { visitor->visit(this); }
  inline const Type* get_type() const override { return T; }

  /// Gets the value of this character expression.
  inline char get_value() const { return value; }

  /// Returns a string representation of this character expression.
  const std::string to_string() override;
};


/// StringLiteral - Represents a string literal expression.
///
/// @example `"hello, world"`, `"foo"`, `"bar"`
class StringLiteral final : public Expr
{
private:
  const std::string value;
  const Type *T;

public:
  StringLiteral(const std::string &value, const Type *T) : value(value), T(T){};
  void pass(ASTVisitor *visitor) override { visitor->visit(this); }
  inline const Type* get_type() const override { return T; }

  /// Gets the value of this string expression.
  inline const std::string get_value() const { return value; }

  /// Returns a string representation of this string expression.
  const std::string to_string() override;
};


/// DeclRefExpr - Represents a reference to a declaration.
///
/// @example `x`, `y`, `z`
class DeclRefExpr final : public Expr
{
private:
  const std::string ident;
  const Type *T;

public:
  DeclRefExpr(const std::string &ident, const Type *T) : ident(ident), T(T){};
  void pass(ASTVisitor *visitor) override { visitor->visit(this); }
  inline const Type* get_type() const override { return T; }

  /// Gets the identifier of this reference expression.
  inline const std::string get_ident() const { return ident; }

  /// Returns a string representation of this reference expression.
  const std::string to_string() override;
};


/// BinaryExpr - Represents a binary expression.
///
/// @example `x + y`, `x - y`, `x * y`
class BinaryExpr final : public Expr
{
private:
  const BinaryOp op;
  std::unique_ptr<Expr> lhs;
  std::unique_ptr<Expr> rhs;
  const Type *T;

public:
  BinaryExpr(const BinaryOp op, std::unique_ptr<Expr> lhs, std::unique_ptr<Expr> rhs) : op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {
    if (this->lhs && this->rhs) {
      T = this->lhs->get_type() == this->rhs->get_type() ? this->lhs->get_type() : nullptr;
    } else {
      T = this->lhs->get_type();
    }
  }
  void pass(ASTVisitor *visitor) override { visitor->visit(this); }

  /// Returns the type of this binary expression. Returns `nullptr` if the operand types mismatch.
  inline const Type* get_type() const override { return T; }

  /// Gets the operator of this binary expression.
  inline const BinaryOp get_op() const { return op; }

  /// Gets the left-hand side of this binary expression.
  inline std::unique_ptr<Expr> get_lhs() { return std::move(lhs); }

  /// Gets the right-hand side of this binary expression.
  inline std::unique_ptr<Expr> get_rhs() { return std::move(rhs); }

  /// Returns a string representation of this binary expression.
  const std::string to_string() override;
};


/// UnaryExpr - Represents a unary expression.
///
/// @example `!x`, `y++`, `#z`
class UnaryExpr final : public Expr
{
private:
  const UnaryOp op;
  std::unique_ptr<Expr> expr;
  const Type *T;

public:
  UnaryExpr(const UnaryOp op, std::unique_ptr<Expr> expr) : op(op), expr(std::move(expr)), T(this->expr->get_type()){};
  void pass(ASTVisitor *visitor) override { visitor->visit(this); }
  inline const Type* get_type() const override { return T; }

  /// Gets the operator of this unary expression.
  inline const UnaryOp get_op() const { return op; }

  /// Gets the expr of this unary expression.
  inline std::unique_ptr<Expr> get_expr() { return std::move(expr); }

  /// Returns a string representation of this unary expression.
  const std::string to_string() override;
};


/// InitExpr - Represents an initialization expression.
///
/// @example `Foo { x: 1, y: 2 }`, `Bar { z: 3, w: 4 }`
class InitExpr final : public Expr
{
private:
  const Type *T;
  std::vector<std::pair<std::string, std::unique_ptr<Expr>>> fields;

public:
  InitExpr(const Type *T, std::vector<std::pair<std::string, std::unique_ptr<Expr>>> fields)
    : T(T), fields(std::move(fields)){};
  void pass(ASTVisitor *visitor) override { visitor->visit(this); }
  inline const Type* get_type() const override { return T; }

  /// Gets the fields of this initialization expression.
  inline std::vector<std::pair<std::string, std::unique_ptr<Expr>>>& get_fields() { return fields; }

  /// Returns a string representation of this initialization expression.
  const std::string to_string() override;
};


/// CallExpr - Represents a function call expression.
///
/// @example `foo()`, `bar(x, y, 3)`
class CallExpr : public Expr
{
protected:
  const std::string callee;
  std::vector<std::unique_ptr<Expr>> args;
  const Type* T;

public:
  CallExpr(const std::string &callee, std::vector<std::unique_ptr<Expr>> args)
    : callee(callee), args(std::move(args)), T(nullptr){};
  void pass(ASTVisitor *visitor) override { visitor->visit(this); }

  /// Returns the type of this function call expression. Returns `nullptr` if the callee is undefined yet.
  inline const Type* get_type() const override { return T; }

  /// Gets the callee of this function call expression.
  inline const std::string get_callee() const { return callee; }

  /// Sets the type of this function call expression.
  inline void set_type(Type *T) { this->T = T; }

  /// Returns a string representation of this function call expression.
  const std::string to_string() override;
};


/// MemberExpr - Represents member access expressions.
///
/// @example `foo.bar`, `baz.qux`
class MemberExpr final : public Expr
{
private:
  std::unique_ptr<Expr> base;
  const std::string member;
  const Type *T;

public:
  MemberExpr(std::unique_ptr<Expr> base, const std::string &member)
    : base(std::move(base)), member(member), T(nullptr){};
  void pass(ASTVisitor *visitor) override { visitor->visit(this); }

  /// Returns the type of this member access expression. Returns `nullptr` if the member is undefined yet.
  inline const Type* get_type() const override { return T; }

  /// Gets the base of this member access expression.
  inline std::unique_ptr<Expr> get_base() { return std::move(base); }

  /// Sets the type of this member access expression.
  inline void set_type(Type *T) { this->T = T; }

  /// Gets the member of this member access expression.
  inline const std::string get_member() const { return member; }

  /// Returns a string representation of this member access expression.
  const std::string to_string() override;
};


/// MemberCallExpr - Represents a member call expression.
///
/// @example `foo.bar()`, `baz.qux()`
class MemberCallExpr final : public CallExpr
{
private:
  std::unique_ptr<Expr> base;

public:
  MemberCallExpr(std::unique_ptr<Expr> base, const std::string &callee, std::vector<std::unique_ptr<Expr>> args)
    : base(std::move(base)), CallExpr(callee, std::move(args)) {};
  void pass(ASTVisitor *visitor) override { visitor->visit(this); }

  /// Gets the base of this member call expression.
  inline std::unique_ptr<Expr> get_base() { return std::move(base); }

  /// Gets the callee of this member call expression.
  inline const std::string get_callee() const { return callee; }

  /// Returns a string representation of this member call expression.
  const std::string to_string() override;
};


/// ArrayAccessExpr - Represents an array access expression.
///
/// @example `foo[0]`, `bar[1]`
class ArrayAccessExpr final : public Expr
{
private:
  std::unique_ptr<Expr> base;
  std::unique_ptr<Expr> index;

public:
  ArrayAccessExpr(std::unique_ptr<Expr> base, std::unique_ptr<Expr> index)
    : base(std::move(base)), index(std::move(index)){};
  void pass(ASTVisitor *visitor) override { visitor->visit(this); }
  inline const Type* get_type() const override { return base->get_type(); }

  /// Gets the base of this array access expression.
  inline std::unique_ptr<Expr> get_base() { return std::move(base); }

  /// Gets the index of this array access expression.
  inline std::unique_ptr<Expr> get_index() { return std::move(index); }

  /// Returns a string representation of this array access expression.
  const std::string to_string() override;
};

#endif  // EXPR_STATIMC_H
