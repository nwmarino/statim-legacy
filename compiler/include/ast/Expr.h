#ifndef EXPR_STATIMC_H
#define EXPR_STATIMC_H

/// Expression AST nodes.
/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include "Stmt.h"

typedef enum {
  /// `!`
  Bang,

  /// `#`
  Rune,

  /// `@`
  Ref,

  /// `...`
  Ellipse,

  /// `.`
  Access,
} UnaryOp;

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

  /// `^^`
  LogicXor,

  /// `&`
  BitAnd,

  /// `|`
  BitOr,

  /// `^`
  BitXor,

  /// `<`
  Lt,

  /// `<=`
  LtEquals,

  /// `>`
  Gt,

  /// `>=`
  GtEquals,

  /// `<<`
  BitLeftShift,

  /// `>>`
  BitRightShift,

  /// `+`
  Plus,

  /// `-`
  Minus,

  /// `*`
  Mult,

  /// `/`
  Div,
} BinaryOp;

/// Base class for expressions; statements that may have a value and type.
class Expr : public Stmt
{
  public:
      virtual ~Expr() = default;
      const virtual std::string to_string(int n) = 0;
};


/// This class represents the `null` literal expression.
class NullExpr final : public Expr
{
  public:
    /// Constructor for null expressions.
    NullExpr() {};

    /// Returns a string representation of this null expression.
    [[nodiscard]]
    const std::string to_string(int n);
};


/// This class represents a default expression in some match statement.
class DefaultExpr final : public Expr
{
  public:
    DefaultExpr() {};

    /// Returns a string representation of this expression.
    [[nodiscard]]
    const std::string to_string(int n);
};


/// This class represents a boolean literal expression.
///
/// @example `true`, `false`
class BooleanLiteral final : public Expr
{
  private:
    bool value;

  public:
    /// Constructor for boolean expressions.
    BooleanLiteral(bool value) : value(value) {};

    /// Gets the value of this boolean expression.
    [[nodiscard]]
    inline bool get_value() const { return value; }

    /// Returns a string representation of this boolean expression.
    [[nodiscard]]
    const std::string to_string(int n);
};


/// This class represents an integer literal expression.
///
/// @example `0`, `512`, `1024`
class IntegerLiteral final : public Expr
{
  private:
    const int value;

  public:
    /// Constructor for integer expressions.
    IntegerLiteral(int value) : value(value) {};

    /// Gets the value of this integer expression.
    [[nodiscard]]
    inline int get_value() const { return value; }

    /// Returns a string representation of this integer expression.
    [[nodiscard]]
    const std::string to_string(int n);
};


/// This class represents a floating point literal expression.
///
/// @example `0.0`, `3.14`, `6.28`
class FPLiteral final : public Expr
{
  private:
    const double value;

  public:
    /// Constructor for floating point expressions.
    FPLiteral(double value) : value(value) {};

    /// Gets the value of this floating point expression.
    [[nodiscard]]
    inline double get_value() const { return value; }

    /// Returns a string representation of this floating point expression.
    [[nodiscard]]
    const std::string to_string(int n);
};


/// This class represents a character literal expression.
///
/// @example `'a'`, `'b'`, `'c'`
class CharLiteral final : public Expr
{
  private:
    const char value;

  public:
    /// Constructor for character expressions.
    CharLiteral(char value) : value(value) {};

    /// Gets the value of this character expression.
    [[nodiscard]]
    inline char get_value() const { return value; }

    /// Returns a string representation of this character expression.
    [[nodiscard]]
    const std::string to_string(int n);
};


/// This class represents a string literal expression.
///
/// @example `"hello, world"`, `"foo"`, `"bar"`
class StringLiteral final : public Expr
{
  private:
    const std::string value;

  public:
    /// Constructor for string expressions.
    StringLiteral(const std::string &value) : value(value) {};

    /// Gets the value of this string expression.
    [[nodiscard]]
    inline const std::string get_value() const { return value; }

    /// Returns a string representation of this string expression.
    [[nodiscard]]
    const std::string to_string(int n);
};


/// This class represents a variable expression.
///
/// @example `x`, `y`, `z`
class VarExpr final : public Expr
{
  private:
    const std::string ident;

  public:
    /// Constructor for variable expressions.
    VarExpr(const std::string &ident) : ident(ident) {};

    /// Gets the identifier of this variable expression.
    [[nodiscard]]
    inline const std::string get_ident() const { return ident; }

    /// Returns a string representation of this variable expression.
    [[nodiscard]]
    const std::string to_string(int n);
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

  public:
    /// Constructor for binary expressions.
    BinaryExpr(const BinaryOp op, std::unique_ptr<Expr> lhs, std::unique_ptr<Expr> rhs)
      : op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {};

    /// Gets the operator of this binary expression.
    [[nodiscard]]
    inline const BinaryOp get_op() const { return op; }

    /// Gets the left-hand side of this binary expression.
    [[nodiscard]]
    inline std::unique_ptr<Expr> get_lhs() { return std::move(lhs); }

    /// Gets the right-hand side of this binary expression.
    [[nodiscard]]
    inline std::unique_ptr<Expr> get_rhs() { return std::move(rhs); }

    /// Returns a string representation of this binary expression.
    [[nodiscard]]
    const std::string to_string(int n);
};


/// This class represents a unary expression.
///
/// @example `!x`, `y++`, `#z`
class UnaryExpr final : public Expr
{
  private:
    const UnaryOp op;
    std::unique_ptr<Expr> expr;

  public:
    /// Constructor for unary expressions.
    UnaryExpr(const UnaryOp op, std::unique_ptr<Expr> expr) : op(op), expr(std::move(expr)) {};

    /// Gets the operator of this unary expression.
    [[nodiscard]]
    inline const UnaryOp get_op() const { return op; }

    /// Gets the expr of this unary expression.
    [[nodiscard]]
    inline std::unique_ptr<Expr> get_expr() { return std::move(expr); }

    /// Returns a string representation of this unary expression.
    [[nodiscard]]
    const std::string to_string(int n);
};


/// This class represents an initialization expression.
///
/// @example `Foo { x: 1, y: 2 }`, `Bar { z: 3, w: 4 }`
class InitExpr final : public Expr
{
  private:
    const std::string ident;
    std::vector<std::pair<std::string, std::unique_ptr<Expr>>> fields;

  public:
    /// Constructor for initialization expressions.
    InitExpr(const std::string &ident, std::vector<std::pair<std::string, std::unique_ptr<Expr>>> fields)
      : ident(ident), fields(std::move(fields)) {};

    /// Gets the identifier of this initialization expression.
    [[nodiscard]]
    inline const std::string get_ident() const { return ident; }

    /// Gets the fields of this initialization expression.
    [[nodiscard]]
    inline std::vector<std::pair<std::string, std::unique_ptr<Expr>>>& get_fields() { return fields; }

    /// Returns a string representation of this initialization expression.
    [[nodiscard]]
    const std::string to_string(int n);
};


/// This class represents a function call expression.
///
/// @example `foo()`, `bar(x, y, 3)`
class CallExpr : public Expr
{
  protected:
    const std::string callee;
    std::vector<std::unique_ptr<Expr>> args;

  public:
    /// Constructor for function call expressions.
    CallExpr(const std::string &callee, std::vector<std::unique_ptr<Expr>> args)
      : callee(callee), args(std::move(args)) {};

    /// Gets the callee of this function call expression.
    [[nodiscard]]
    inline const std::string get_callee() const { return callee; }

    /// Returns a string representation of this function call expression.
    [[nodiscard]]
    const std::string to_string(int n);
};


/// This class represents member access expressions.
///
/// @example `foo.bar`, `baz.qux`
class MemberExpr final : public Expr
{
  private:
    std::unique_ptr<Expr> base;
    const std::string member;

  public:
    /// Constructor for member access expressions.
    MemberExpr(std::unique_ptr<Expr> base, const std::string &member)
      : base(std::move(base)), member(member) {};

    /// Gets the base of this member access expression.
    [[nodiscard]]
    inline std::unique_ptr<Expr> get_base() { return std::move(base); }

    /// Gets the member of this member access expression.
    [[nodiscard]]
    inline const std::string get_member() const { return member; }

    /// Returns a string representation of this member access expression.
    [[nodiscard]]
    const std::string to_string(int n);
};


/// This class represents member call expressions.
///
/// @example `foo.bar()`, `baz.qux()`
class MemberCallExpr : public CallExpr
{
  private:
    std::unique_ptr<Expr> base;

  public:
    /// Constructor for member call expressions.
    MemberCallExpr(std::unique_ptr<Expr> base, const std::string &callee, std::vector<std::unique_ptr<Expr>> args)
      : base(std::move(base)), CallExpr(callee, std::move(args)) {};

    /// Gets the base of this member call expression.
    [[nodiscard]]
    inline std::unique_ptr<Expr> get_base() { return std::move(base); }

    /// Gets the callee of this member call expression.
    [[nodiscard]]
    inline const std::string get_callee() const { return callee; }

    /// Returns a string representation of this member call expression.
    [[nodiscard]]
    const std::string to_string(int n);
};

#endif  // EXPR_STATIMC_H
