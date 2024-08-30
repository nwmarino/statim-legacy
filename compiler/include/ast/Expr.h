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
class NullExpr : public Expr
{
  public:
    /// Constructor for null expressions.
    NullExpr() {};

    /// Returns a string representation of this null expression.
    [[nodiscard]]
    const std::string to_string(int n);
};


/// This class represents a default expression in some match statement.
class DefaultExpr : public Expr
{
  public:
    DefaultExpr() {};

    /// Returns a string representation of this expression.
    [[nodiscard]]
    const std::string to_string(int n);
};


/// This class represents a boolean literla expression.
///
/// @example `true`, `false`
class BooleanExpr : public Expr
{
  private:
    bool value;

  public:
    /// Constructor for boolean expressions.
    BooleanExpr(bool value) : value(value) {};

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
class IntegerExpr : public Expr
{
  private:
    const int value;

  public:
    /// Constructor for integer expressions.
    IntegerExpr(int value) : value(value) {};

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
class FloatingPointExpr : public Expr
{
  private:
    const double value;

  public:
    /// Constructor for floating point expressions.
    FloatingPointExpr(double value) : value(value) {};

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
class CharExpr : public Expr
{
  private:
    const char value;

  public:
    /// Constructor for character expressions.
    CharExpr(char value) : value(value) {};

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
class StringExpr : public Expr
{
  private:
    const std::string value;

  public:
    /// Constructor for string expressions.
    StringExpr(const std::string &value) : value(value) {};

    /// Gets the value of this string expression.
    [[nodiscard]]
    inline const std::string get_value() const { return value; }

    /// Returns a string representation of this string expression.
    [[nodiscard]]
    const std::string to_string(int n);
};


/// This class represents a byte literal expression.
///
/// @example `b'a'`, `b'b'`, `b'c'`
class ByteExpr : public Expr
{
  private:
    const char value;

  public:
    /// Constructor for byte expressions.
    ByteExpr(char value) : value(value) {};

    /// Gets the value of this byte expression.
    [[nodiscard]]
    inline char get_value() const { return value; }

    /// Returns a string representation of this byte expression.
    [[nodiscard]]
    const std::string to_string(int n);
};


/// This class represents a byte string literal expression.
///
/// @example `b"hello, world"`, `b"foo"`, `b"bar"`
class ByteStringExpr : public Expr
{
  private:
    const std::string value;

  public:
    /// Constructor for byte string expressions.
    ByteStringExpr(const std::string &value) : value(value) {};

    /// Gets the value of this byte string expression.
    [[nodiscard]]
    inline const std::string get_value() const { return value; }

    /// Returns a string representation of this byte string expression.
    [[nodiscard]]
    const std::string to_string(int n);
};


/// This class represents a variable expression.
///
/// @example `x`, `y`, `z`
class VariableExpr : public Expr
{
  private:
    const std::string ident;

  public:
    /// Constructor for variable expressions.
    VariableExpr(const std::string &ident) : ident(ident) {};

    /// Gets the identifier of this variable expression.
    [[nodiscard]]
    inline const std::string get_ident() const { return ident; }

    /// Returns a string representation of this variable expression.
    [[nodiscard]]
    const std::string to_string(int n);
};


/// This class represents a function call expression.
///
/// @example `foo()`, `bar(x, y, 3)`
class CallExpr : public Expr
{
  private:
    std::string callee;
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


/// This class represents a binary expression.
///
/// @example `x + y`, `x - y`, `x * y`
class BinaryExpr : public Expr
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
class UnaryExpr : public Expr
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
class InitExpr : public Expr
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

#endif  // EXPR_STATIMC_H
