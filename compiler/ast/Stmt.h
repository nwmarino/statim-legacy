#ifndef STMT_STATIMC_H
#define STMT_STATIMC_H

/// Statement AST nodes.
/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <memory>
#include <string>
#include <utility>
#include <vector>

/// Base class for a statement representation.
class Stmt
{
  public:
      virtual ~Stmt() = default;
      const virtual std::string to_string(int n) = 0;
};


/// Base class for expressions; statements that may have a value and type.
class Expr : public Stmt
{
  public:
      virtual ~Expr() = default;
      const virtual std::string to_string(int n) = 0;
};


/// This class represents a list of statements.
class CompoundStmt : public Stmt
{
  std::vector<std::unique_ptr<Stmt>> stmts;

  public:
    /// Constructor for compound statements.
    CompoundStmt(std::vector<std::unique_ptr<Stmt>> stmts) : stmts(std::move(stmts)) {};

    /// Determine if the body of this compound statement is empty.
    [[nodiscard]]
    inline bool is_empty() const { return stmts.empty(); }

    /// Gets the statements of this compound statement.
    [[nodiscard]]
    inline const std::vector<std::unique_ptr<Stmt>> get_stmts() const { return stmts; }

    /// Returns a string representation of this compound statement.
    [[nodiscard]]
    const std::string to_string(int n);
};


/// This class represents an if statement.
class IfStmt : public Stmt
{
  private:
    std::unique_ptr<Expr> cond;
    std::unique_ptr<Stmt> then_body;
    std::unique_ptr<Stmt> else_body;

  public:
    /// Constructor for if statements with an else body.
    IfStmt(std::unique_ptr<Expr> cond, std::unique_ptr<Stmt> then_body, std::unique_ptr<Stmt> else_body)
      : cond(std::move(cond)), then_body(std::move(then_body)), else_body(std::move(else_body)) {};

    /// Constructor for if statements without an else body.
    IfStmt(std::unique_ptr<Expr> cond, std::unique_ptr<Stmt> then_body)
      : cond(std::move(cond)), then_body(std::move(then_body)), else_body(nullptr) {};

    /// Determine if this if statement has an else body.
    [[nodiscard]]
    inline bool has_else() const { return else_body != nullptr; }

    /// Gets the condition of this if statement.
    [[nodiscard]]
    inline std::unique_ptr<Expr> get_cond() { return std::move(cond); }

    /// Gets the then body of this if statement.
    [[nodiscard]]
    inline std::unique_ptr<Stmt> get_then_body() { return std::move(then_body); }

    /// Gets the else body of this if statement.
    [[nodiscard]]
    inline std::unique_ptr<Stmt> get_else_body() { return std::move(else_body); }

    /// Returns a string representation of this if statement.
    [[nodiscard]]
    const std::string to_string(int n);
};


/// Pattern matching related classes.
///
/// These classes are used to represent pattern matching constructs in the AST.

/// This class represents a default expression in some match statement.
class DefaultExpr : public Expr
{
  public:
    DefaultExpr() {};

    /// Returns a string representation of this expression.
    [[nodiscard]]
    const std::string to_str(int n);
};

/// This class represents a possible pattern matching class.
class MatchCase : public Stmt
{
  private:
    std::unique_ptr<Expr> expr;
    std::unique_ptr<Stmt> body;

  public:
    /// Constructor for match cases.
    MatchCase(std::unique_ptr<Expr> expr, std::unique_ptr<Stmt> body)
      : expr(std::move(expr)), body(std::move(body)) {};

    /// Determine if this is the default case.
    [[nodiscard]]
    inline bool is_default() const { return typeid(expr) == typeid(DefaultExpr); }

    /// Gets the expression of this match case.
    [[nodiscard]]
    inline std::unique_ptr<Expr> get_expr() { return std::move(expr); }

    /// Gets the body of this match case.
    [[nodiscard]]
    inline std::unique_ptr<Stmt> get_body() { return std::move(body); }

    /// Returns a string representation of this match case.
    [[nodiscard]]
    const std::string to_string(int n);
};

/// This class represents the structure of a match statement.
class MatchStmt : public Stmt
{
  private:
    std::unique_ptr<Expr> expr;
    std::vector<std::unique_ptr<MatchCase>> cases;

  public:
    /// Constructor for match statements.
    MatchStmt(std::unique_ptr<Expr> expr, std::vector<std::unique_ptr<MatchCase>> cases)
      : expr(std::move(expr)), cases(std::move(cases)) {};

    /// Gets the expression of this match statement.
    [[nodiscard]]
    inline std::unique_ptr<Expr> get_expr() { return std::move(expr); }

    /// Gets the cases of this match statement.
    [[nodiscard]]
    inline std::vector<std::unique_ptr<MatchCase>> get_cases() { return std::move(cases); }

    /// Returns a string representation of this match statement.
    [[nodiscard]]
    const std::string to_string(int n);
};


/// This class represents a function return statement.
class ReturnStmt : public Stmt
{
  private:
    std::unique_ptr<Expr> expr;

  public:
    /// Constructor for return statements with an expression.
    ReturnStmt(std::unique_ptr<Expr> expr) : expr(std::move(expr)) {};

    /// Determine if this return statement has an expression.
    [[nodiscard]]
    inline bool has_expr() const { return expr != nullptr; }

    /// Gets the expression of this return statement.
    [[nodiscard]]
    inline std::unique_ptr<Expr> get_expr() { return std::move(expr); }

    /// Returns a string representation of this return statement.
    [[nodiscard]]
    const std::string to_string(int n);
};


/// This class represents a looping until statement.
class UntilStmt : public Stmt
{
  private:
    std::unique_ptr<Expr> cond;
    std::unique_ptr<Stmt> body;

  public:
    /// Constructor for until statements.
    UntilStmt(std::unique_ptr<Expr> cond, std::unique_ptr<Stmt> body)
      : cond(std::move(cond)), body(std::move(body)) {};

    /// Gets the condition of this until statement.
    [[nodiscard]]
    inline std::unique_ptr<Expr> get_cond() { return std::move(cond); }

    /// Gets the body of this until statement.
    [[nodiscard]]
    inline std::unique_ptr<Stmt> get_body() { return std::move(body); }

    /// Returns a string representation of this until statement.
    [[nodiscard]]
    const std::string to_string(int n);
};

#endif  // STMT_STATIMC_H
