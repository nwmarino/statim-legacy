#ifndef STMT_STATIMC_H
#define STMT_STATIMC_H

/// Statement AST nodes.
/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "../sema/ASTVisitor.h"

class Scope;
class Decl;
class Expr;

/// Base class for a statement representation.
class Stmt
{
public:
  virtual ~Stmt() = default;
  virtual void pass(ASTVisitor *visitor) = 0;
  const virtual std::string to_string() = 0;
};


/// Declarative statements that mix expressions and declarations.
///
/// In particular, these statements are used to declare variables within a scope.
class DeclStmt : public Stmt
{
private:
  std::unique_ptr<Decl> decl;

public:
  DeclStmt(std::unique_ptr<Decl> decl) : decl(std::move(decl)) {};

  void pass(ASTVisitor *visitor) override { visitor->visit(this); }

  /// Returns a string representation of this declaration statement.
  const std::string to_string() override;
};


/// This class represents a list of statements.
class CompoundStmt final : public Stmt
{
private:
  std::vector<std::unique_ptr<Stmt>> stmts;
  std::shared_ptr<Scope> scope;

public:
  CompoundStmt(std::vector<std::unique_ptr<Stmt>> stmts, std::shared_ptr<Scope> scope) : stmts(std::move(stmts)), scope(scope) {};

  void pass(ASTVisitor *visitor) override { visitor->visit(this); }

  /// Determine if the body of this compound statement is empty.
  inline bool is_empty() const { return stmts.empty(); }

  /// Returns the scope of this compound statement.
  inline std::shared_ptr<Scope> get_scope() const { return scope; }

  /// Returns a string representation of this compound statement.
  const std::string to_string() override;
};


/// This class represents an if statement.
class IfStmt final : public Stmt
{
private:
  std::unique_ptr<Expr> cond;
  std::unique_ptr<Stmt> then_body;
  std::unique_ptr<Stmt> else_body;

public:
  IfStmt(std::unique_ptr<Expr> cond, std::unique_ptr<Stmt> then_body, std::unique_ptr<Stmt> else_body)
    : cond(std::move(cond)), then_body(std::move(then_body)), else_body(std::move(else_body)) {};

  IfStmt(std::unique_ptr<Expr> cond, std::unique_ptr<Stmt> then_body)
    : cond(std::move(cond)), then_body(std::move(then_body)), else_body(nullptr) {};

  void pass(ASTVisitor *visitor) override { visitor->visit(this); }

  /// Determine if this if statement has an else body.
  inline bool has_else() const { return else_body != nullptr; }

  /// Returns a string representation of this if statement.
  const std::string to_string() override;
};


/// Pattern matching related classes.
///
/// These classes are used to represent pattern matching constructs in the AST.

/// This class represents a possible pattern matching class.
class MatchCase final : public Stmt
{
private:
  std::unique_ptr<Expr> expr;
  std::unique_ptr<Stmt> body;

public:
  MatchCase(std::unique_ptr<Expr> expr, std::unique_ptr<Stmt> body)
    : expr(std::move(expr)), body(std::move(body)) {};

  void pass(ASTVisitor *visitor) override { visitor->visit(this); }

  /// Returns a string representation of this match case.
  const std::string to_string() override;
};


/// This class represents the structure of a match statement.
class MatchStmt final : public Stmt
{
private:
  std::unique_ptr<Expr> expr;
  std::vector<std::unique_ptr<MatchCase>> cases;

public:
  MatchStmt(std::unique_ptr<Expr> expr, std::vector<std::unique_ptr<MatchCase>> cases)
    : expr(std::move(expr)), cases(std::move(cases)) {};

  void pass(ASTVisitor *visitor) override { visitor->visit(this); }

  /// Returns a string representation of this match statement.
  const std::string to_string() override;
};


/// This class represents a function return statement.
class ReturnStmt final : public Stmt
{
private:
  std::unique_ptr<Expr> expr;

public:
  ReturnStmt(std::unique_ptr<Expr> expr) : expr(std::move(expr)) {};
  
  void pass(ASTVisitor *visitor) override { visitor->visit(this); }

  /// Determine if this return statement has an expression.
  inline bool has_expr() const { return expr != nullptr; }

  /// Returns a string representation of this return statement.
  const std::string to_string() override;
};


/// This class represents a looping until statement.
class UntilStmt final : public Stmt
{
private:
  std::unique_ptr<Expr> cond;
  std::unique_ptr<Stmt> body;

public:
  UntilStmt(std::unique_ptr<Expr> cond, std::unique_ptr<Stmt> body)
    : cond(std::move(cond)), body(std::move(body)) {};

  void pass(ASTVisitor *visitor) override { visitor->visit(this); }

  /// Returns a string representation of this until statement.
  const std::string to_string() override;
};

#endif  // STMT_STATIMC_H
