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
  const virtual std::string get_type_id() const = 0;
  const virtual std::string to_string() = 0;
};


/// This class represents the `null` literal expression.
class NullExpr final : public Expr
{
private:
  Type *__type;

public:
  NullExpr(){};

  void pass(ASTVisitor *visitor) override { visitor->visit(this); }

  /// Gets the type of this null expression.
  inline const std::string get_type_id() const override { return "null"; }

  /// Returns a string representation of this null expression.
  const std::string to_string() override;
};


/// This class represents a default expression in some match statement.
class DefaultExpr final : public Expr
{
private:
  Type *__type;

public:
  DefaultExpr(){};
  void pass(ASTVisitor *visitor) override { visitor->visit(this); }

  /// Gets the type of this default expression.
  inline const std::string get_type_id() const override { return "default"; }

  /// Returns a string representation of this expression.
  const std::string to_string() override;
};


/// This class represents a boolean literal expression.
///
/// @example `true`, `false`
class BooleanLiteral final : public Expr
{
private:
  const unsigned int value;
  Type *__type;

public:
  BooleanLiteral(bool value) : value(value ? 1 : 0){};
  void pass(ASTVisitor *visitor) override { visitor->visit(this); }

  /// Gets the value of this boolean expression.
  inline bool get_value() const { return value; }

  /// Gets the type of this boolean expression.
  inline const std::string get_type_id() const override { return "bool"; }

  /// Returns a string representation of this boolean expression.
  const std::string to_string() override;
};


/// This class represents an integer literal expression.
///
/// @example `0`, `512`, `1024`
class IntegerLiteral final : public Expr
{
private:
  const long value;
  const bool signedness;
  const std::string type_ident;
  Type *__type;

public:
  IntegerLiteral(int value, const std::string &type_ident) : value(value), signedness(value < 0), type_ident(type_ident){};
  void pass(ASTVisitor *visitor) override { visitor->visit(this); }

  /// Gets the value of this integer expression.
  inline int get_value() const { return value; }

  /// Gets the signedness of this integer expression.
  inline bool is_signed() const { return signedness; }

  /// Gets the type of this integer expression.
  inline const std::string get_type_id() const override { return type_ident; }

  /// Returns a string representation of this integer expression.
  const std::string to_string() override;
};


/// This class represents a floating point literal expression.
///
/// @example `0.0`, `3.14`, `6.28`
class FPLiteral final : public Expr
{
private:
  const double value;
  Type *__type;

public:
  FPLiteral(double value) : value(value), type("float") {};

  void pass(ASTVisitor *visitor) override { visitor->visit(this); }

  /// Gets the value of this floating point expression.
  inline double get_value() const { return value; }

  /// Gets the type of this floating point expression.
  inline const std::string get_type() const override { return type; }

  /// Returns a string representation of this floating point expression.
  const std::string to_string() override;
};


/// This class represents a character literal expression.
///
/// @example `'a'`, `'b'`, `'c'`
class CharLiteral final : public Expr
{
private:
  const char value;
  const std::string type;

public:
  CharLiteral(char value) : value(value), type("char") {};

  void pass(ASTVisitor *visitor) override { visitor->visit(this); }

  /// Gets the value of this character expression.
  inline char get_value() const { return value; }

  /// Gets the type of this character expression.
  inline const std::string get_type() const override { return type; }

  /// Returns a string representation of this character expression.
  const std::string to_string() override;
};


/// This class represents a string literal expression.
///
/// @example `"hello, world"`, `"foo"`, `"bar"`
class StringLiteral final : public Expr
{
private:
  const std::string value;
  const std::string type;

public:
  StringLiteral(const std::string &value) : value(value), type("str") {};

  void pass(ASTVisitor *visitor) override { visitor->visit(this); }

  /// Gets the value of this string expression.
  inline const std::string get_value() const { return value; }

  /// Gets the type of this string expression.
  inline const std::string get_type() const override { return type; }

  /// Returns a string representation of this string expression.
  const std::string to_string() override;
};


/// This class represents a declaration reference expression.
///
/// @example `x`, `y`, `z`
class DeclRefExpr final : public Expr
{
private:
  const std::string ident;
  const std::string type;

public:
  DeclRefExpr(const std::string &ident, const std::string &type) : ident(ident), type(type) {};

  void pass(ASTVisitor *visitor) override { visitor->visit(this); }

  /// Gets the identifier of this reference expression.
  inline const std::string get_ident() const { return ident; }

  /// Gets the type of this reference expression.
  inline const std::string get_type() const override { return type; }

  /// Returns a string representation of this reference expression.
  const std::string to_string() override;
};


/// This class represents a binary expression.
///
/// @example `x + y`, `x - y`, `x * y`
class BinaryExpr final : public Expr
{
private:
  const BinaryOp op;
  std::unique_ptr<Expr> lhs;
  std::unique_ptr<Expr> rhs;
  std::string type;

public:
  BinaryExpr(const BinaryOp op, std::unique_ptr<Expr> lhs, std::unique_ptr<Expr> rhs) : op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {
    if (this->lhs && this->rhs) {
      type = this->lhs->get_type() == this->rhs->get_type() ? this->lhs->get_type() : "mismatch";
    } else {
      type = this->lhs->get_type();
    }
  };

  void pass(ASTVisitor *visitor) override { visitor->visit(this); }

  /// Gets the operator of this binary expression.
  inline const BinaryOp get_op() const { return op; }

  /// Gets the left-hand side of this binary expression.
  inline std::unique_ptr<Expr> get_lhs() { return std::move(lhs); }

  /// Gets the right-hand side of this binary expression.
  inline std::unique_ptr<Expr> get_rhs() { return std::move(rhs); }

  /// Gets the type of this binary expression.
  inline const std::string get_type() const override { return type; }

  /// Returns a string representation of this binary expression.
  const std::string to_string() override;
};


/// This class represents a unary expression.
///
/// @example `!x`, `y++`, `#z`
class UnaryExpr final : public Expr
{
private:
  const UnaryOp op;
  std::unique_ptr<Expr> expr;
  const std::string type;

public:
  UnaryExpr(const UnaryOp op, std::unique_ptr<Expr> expr) : op(op), expr(std::move(expr)), type(this->expr->get_type()) {};

  void pass(ASTVisitor *visitor) override { visitor->visit(this); }

  /// Gets the operator of this unary expression.
  inline const UnaryOp get_op() const { return op; }

  /// Gets the type of this unary expression.
  inline const std::string get_type() const override { return type; }

  /// Gets the expr of this unary expression.
  inline std::unique_ptr<Expr> get_expr() { return std::move(expr); }

  /// Returns a string representation of this unary expression.
  const std::string to_string() override;
};


/// This class represents an initialization expression.
///
/// @example `Foo { x: 1, y: 2 }`, `Bar { z: 3, w: 4 }`
class InitExpr final : public Expr
{
private:
  const std::string type;
  std::vector<std::pair<std::string, std::unique_ptr<Expr>>> fields;

public:
  InitExpr(const std::string &type, std::vector<std::pair<std::string, std::unique_ptr<Expr>>> fields)
    : type(type), fields(std::move(fields)) {};

  void pass(ASTVisitor *visitor) override { visitor->visit(this); }

  /// Gets the identifier of this initialization expression.
  inline const std::string get_type() const override { return type; }

  /// Gets the fields of this initialization expression.
  inline std::vector<std::pair<std::string, std::unique_ptr<Expr>>>& get_fields() { return fields; }

  /// Returns a string representation of this initialization expression.
  const std::string to_string() override;
};


/// This class represents a function call expression.
///
/// @example `foo()`, `bar(x, y, 3)`
class CallExpr : public Expr
{
protected:
  const std::string callee;
  std::vector<std::unique_ptr<Expr>> args;
  std::string type;

public:
  CallExpr(const std::string &callee, std::vector<std::unique_ptr<Expr>> args)
    : callee(callee), args(std::move(args)), type("unknown") {};

  void pass(ASTVisitor *visitor) override { visitor->visit(this); }

  /// Gets the callee of this function call expression.
  inline const std::string get_callee() const { return callee; }

  /// Gets the type of this function call expression.
  inline const std::string get_type() const override { return type; }

  /// Sets the type of this function call expression.
  inline void set_type(const std::string &type) { this->type = type; }

  /// Returns a string representation of this function call expression.
  const std::string to_string() override;
};


/// This class represents member access expressions.
///
/// @example `foo.bar`, `baz.qux`
class MemberExpr final : public Expr
{
private:
  std::unique_ptr<Expr> base;
  const std::string member;
  std::string type;

public:
  MemberExpr(std::unique_ptr<Expr> base, const std::string &member)
    : base(std::move(base)), member(member), type("unknown") {};

  void pass(ASTVisitor *visitor) override { visitor->visit(this); }

  /// Gets the base of this member access expression.
  inline std::unique_ptr<Expr> get_base() { return std::move(base); }

  /// Gets the type of this member access expression.
  inline const std::string get_type() const override { return type; }

  /// Sets the type of this member access expression.
  inline void set_type(const std::string &type) { this->type = type; }

  /// Gets the member of this member access expression.
  inline const std::string get_member() const { return member; }

  /// Returns a string representation of this member access expression.
  [[nodiscard]]
  const std::string to_string() override;
};


/// This class represents member call expressions.
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
  [[nodiscard]]
  const std::string to_string() override;
};


/// This class represents an array access expression.
///
/// @example `foo[0]`, `bar[1]`
class ArrayAccessExpr final : public Expr
{
private:
  std::unique_ptr<Expr> base;
  std::unique_ptr<Expr> index;
  std::string type;

public:
  ArrayAccessExpr(std::unique_ptr<Expr> base, std::unique_ptr<Expr> index)
    : base(std::move(base)), index(std::move(index)), type("unknown") {};

  void pass(ASTVisitor *visitor) override { visitor->visit(this); }

  /// Gets the base of this array access expression.
  inline std::unique_ptr<Expr> get_base() { return std::move(base); }

  /// Gets the index of this array access expression.
  inline std::unique_ptr<Expr> get_index() { return std::move(index); }

  /// Gets the type of this array access expression.
  inline const std::string get_type() const override { return type; }

  /// Sets the type of this array access expression.
  inline void set_type(const std::string &type) { this->type = type; }

  /// Returns a string representation of this array access expression.
  const std::string to_string() override;
};

#endif  // EXPR_STATIMC_H
