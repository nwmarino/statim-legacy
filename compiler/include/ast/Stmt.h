#ifndef STMT_STATIMC_H
#define STMT_STATIMC_H

/// Statement AST nodes.
/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "../sema/ASTVisitor.h"
#include "../token/Token.h"

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
  virtual const Metadata get_meta() const = 0;
};


/// Declarative statements that mix expressions and declarations.
///
/// In particular, these statements are used to declare variables within a scope.
class DeclStmt : public Stmt
{
private:
  std::unique_ptr<Decl> decl;
  const Metadata meta;

public:
  DeclStmt(std::unique_ptr<Decl> decl, const Metadata &meta) : decl(std::move(decl)), meta(meta) {};
  void pass(ASTVisitor *visitor) override { visitor->visit(this); }
  inline Decl* get_decl() const { return decl.get(); }
  const Metadata get_meta() const override { return meta; }

  /// Returns a string representation of this declaration statement.
  const std::string to_string() override;
};


/// This class represents a list of statements.
class CompoundStmt final : public Stmt
{
private:
  std::vector<std::unique_ptr<Stmt>> stmts;
  std::shared_ptr<Scope> scope;
  const Metadata meta;

public:
  CompoundStmt(std::vector<std::unique_ptr<Stmt>> stmts, std::shared_ptr<Scope> scope, const Metadata &meta)
    : stmts(std::move(stmts)), scope(scope), meta(meta){};
  void pass(ASTVisitor *visitor) override { visitor->visit(this); }
  inline std::vector<Stmt *> get_stmts() const {
    std::vector<Stmt *> stmt_ptrs;
    for (const std::unique_ptr<Stmt> &stmt : stmts) {
      stmt_ptrs.push_back(stmt.get());
    }
    return stmt_ptrs;
  }
  const Metadata get_meta() const override { return meta; }

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
  const Metadata meta;

public:
  IfStmt(std::unique_ptr<Expr> cond, std::unique_ptr<Stmt> then_body, std::unique_ptr<Stmt> else_body, const Metadata &meta)
    : cond(std::move(cond)), then_body(std::move(then_body)), else_body(std::move(else_body)), meta(meta){};
  IfStmt(std::unique_ptr<Expr> cond, std::unique_ptr<Stmt> then_body, const Metadata &meta)
    : cond(std::move(cond)), then_body(std::move(then_body)), else_body(nullptr), meta(meta){};
  void pass(ASTVisitor *visitor) override { visitor->visit(this); }
  inline Expr* get_cond() const { return cond.get(); }
  inline Stmt* get_then_body() const { return then_body ? then_body.get() : nullptr; }
  inline Stmt* get_else_body() const { return else_body ? else_body.get() : nullptr; }
  const Metadata get_meta() const override { return meta; }

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
  const Metadata meta;

public:
  MatchCase(std::unique_ptr<Expr> expr, std::unique_ptr<Stmt> body, const Metadata &meta)
    : expr(std::move(expr)), body(std::move(body)), meta(meta){};
  void pass(ASTVisitor *visitor) override { visitor->visit(this); }
  inline Expr* get_expr() const { return expr.get(); }
  inline Stmt* get_body() const { return body.get(); }
  const Metadata get_meta() const override { return meta; }

  /// Returns a string representation of this match case.
  const std::string to_string() override;
};


/// This class represents the structure of a match statement.
class MatchStmt final : public Stmt
{
private:
  std::unique_ptr<Expr> expr;
  std::vector<std::unique_ptr<MatchCase>> cases;
  const Metadata meta;

public:
  MatchStmt(std::unique_ptr<Expr> expr, std::vector<std::unique_ptr<MatchCase>> cases, const Metadata &meta)
    : expr(std::move(expr)), cases(std::move(cases)), meta(meta){};
  void pass(ASTVisitor *visitor) override { visitor->visit(this); }
  inline Expr* get_expr() const { return expr.get(); }
  inline std::vector<MatchCase *> get_cases() const {
    std::vector<MatchCase *> case_ptrs;
    for (auto &c : cases) {
      case_ptrs.push_back(c.get());
    }
    return case_ptrs;
  }
  const Metadata get_meta() const override { return meta; }

  /// Returns a string representation of this match statement.
  const std::string to_string() override;
};


/// This class represents a function return statement.
class ReturnStmt final : public Stmt
{
private:
  std::unique_ptr<Expr> expr;
  const Metadata meta;

public:
  ReturnStmt(std::unique_ptr<Expr> expr, const Metadata &meta) : expr(std::move(expr)), meta(meta){};
  void pass(ASTVisitor *visitor) override { visitor->visit(this); }
  inline Expr* get_expr() const { return expr.get(); }
  const Metadata get_meta() const override { return meta; }

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
  const Metadata meta;

public:
  UntilStmt(std::unique_ptr<Expr> cond, std::unique_ptr<Stmt> body, const Metadata &meta)
    : cond(std::move(cond)), body(std::move(body)), meta(meta){};
  void pass(ASTVisitor *visitor) override { visitor->visit(this); }
  inline Expr* get_cond() const { return cond.get(); }
  inline Stmt* get_body() const { return body.get(); }
  const Metadata get_meta() const override { return meta; }

  /// Returns a string representation of this until statement.
  const std::string to_string() override;
};

#endif  // STMT_STATIMC_H
