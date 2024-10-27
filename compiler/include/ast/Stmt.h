#ifndef STMT_STATIMC_H
#define STMT_STATIMC_H

/// Statement AST nodes.
/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "../core/ASTVisitor.h"
#include "../token/Token.h"

class Scope;
class Decl;
class Expr;

/// Base class for a statement representation.
class Stmt
{
protected:
  const Metadata meta;

public:
  Stmt(const Metadata &meta) : meta(meta) {};
  const Metadata get_meta() const { return meta; }

  virtual void pass(ASTVisitor *visitor) = 0;
  virtual const std::string to_string() = 0;
};


/// Declarative statements that mix expressions and declarations.
///
/// In particular, these statements are used to declare variables within a scope.
class DeclStmt final : public Stmt
{
private:
  std::unique_ptr<Decl> decl;

public:
  DeclStmt(std::unique_ptr<Decl> decl, const Metadata &meta) : Stmt(meta), decl(std::move(decl)) {};
  void pass(ASTVisitor *visitor) override { visitor->visit(this); }

  /// Returns the underlying declaration of this DeclStmt.
  inline Decl* get_decl() const { return decl.get(); }

  /// Returns a string representation of this AST node.
  const std::string to_string() override;
};


/// This class represents a list of statements.
class CompoundStmt final : public Stmt
{
private:
  std::vector<std::unique_ptr<Stmt>> stmts;
  std::shared_ptr<Scope> scope;

public:
  CompoundStmt(std::vector<std::unique_ptr<Stmt>> stmts, std::shared_ptr<Scope> scope, const Metadata &meta)
    : Stmt(meta), stmts(std::move(stmts)), scope(scope) {};
  void pass(ASTVisitor *visitor) override { visitor->visit(this); }

  /// Returns true if this compound statement is empty, and false otherwise.
  inline bool is_empty() const { return stmts.empty(); }

  /// Returns a shared pointer to the scope of this compound statement.
  inline std::shared_ptr<Scope> get_scope() const { return scope; }

  /// Returns a string representation of this AST node.
  const std::string to_string() override;

  /// Push a new statement onto the list of statements.
  inline void push_stmt(std::unique_ptr<Stmt> stmt) { stmts.push_back(std::move(stmt)); }

  /// Returns a vector of pointers to the statements in this compound statement.
  inline std::vector<Stmt *> get_stmts() const {
    std::vector<Stmt *> stmt_ptrs;
    for (const std::unique_ptr<Stmt> &stmt : stmts) {
      stmt_ptrs.push_back(stmt.get());
    }
    return stmt_ptrs;
  }
};


/// This class represents an if statement.
class IfStmt final : public Stmt
{
private:
  std::unique_ptr<Expr> cond;
  std::unique_ptr<Stmt> then_body;
  std::unique_ptr<Stmt> else_body;

public:
  IfStmt(std::unique_ptr<Expr> cond, std::unique_ptr<Stmt> then_body, std::unique_ptr<Stmt> else_body, const Metadata &meta)
    : Stmt(meta), cond(std::move(cond)), then_body(std::move(then_body)), else_body(std::move(else_body)) {};
  IfStmt(std::unique_ptr<Expr> cond, std::unique_ptr<Stmt> then_body, const Metadata &meta)
    : Stmt(meta), cond(std::move(cond)), then_body(std::move(then_body)), else_body(nullptr) {};
  void pass(ASTVisitor *visitor) override { visitor->visit(this); }

  /// Returns the condition expression of this if statement.
  inline Expr* get_cond() const { return cond.get(); }

  /// Returns the then body of this if statement.
  inline Stmt* get_then_body() const { return then_body ? then_body.get() : nullptr; }

  /// Returns the else body of this if statement.
  inline Stmt* get_else_body() const { return else_body ? else_body.get() : nullptr; }

  /// Returns true if this if statement has an else body, and false otherwise.
  inline bool has_else() const { return else_body != nullptr; }

  /// Returns a string representation of this AST node.
  const std::string to_string() override;
};


/// This class represents a possible pattern matching class.
class MatchCase final : public Stmt
{
private:
  std::unique_ptr<Expr> expr;
  std::unique_ptr<Stmt> body;

public:
  MatchCase(std::unique_ptr<Expr> expr, std::unique_ptr<Stmt> body, const Metadata &meta)
    : Stmt(meta), expr(std::move(expr)), body(std::move(body)) {};
  void pass(ASTVisitor *visitor) override { visitor->visit(this); }

  /// Returns the expression of this match case.
  inline Expr* get_expr() const { return expr.get(); }

  /// Returns the body of this match case.
  inline Stmt* get_body() const { return body.get(); }

  /// Returns a string representation of this AST node.
  const std::string to_string() override;
};


/// This class represents the structure of a match statement.
class MatchStmt final : public Stmt
{
private:
  std::unique_ptr<Expr> expr;
  std::vector<std::unique_ptr<MatchCase>> cases;

public:
  MatchStmt(std::unique_ptr<Expr> expr, std::vector<std::unique_ptr<MatchCase>> cases, const Metadata &meta)
    : Stmt(meta), expr(std::move(expr)), cases(std::move(cases)) {};
  void pass(ASTVisitor *visitor) override { visitor->visit(this); }

  /// Returns the expression of this match statement.
  inline Expr* get_expr() const { return expr.get(); }

  /// Returns the number of cases in this match case.
  inline std::size_t get_num_cases() const { return cases.size(); }

  /// Returns a string representation of this AST node.
  const std::string to_string() override;

  /// Returns a vector of pointers to the cases in this match statement.
  inline std::vector<MatchCase *> get_cases() const {
    std::vector<MatchCase *> case_ptrs;
    for (const std::unique_ptr<MatchCase> &c : cases) {
      case_ptrs.push_back(c.get());
    }
    return case_ptrs;
  }

  /// Returns a vector pointers to all cases but the default case.
  inline std::vector<MatchCase *> get_cases_no_default() const {
    std::vector<MatchCase *> case_ptrs;
    for (const std::unique_ptr<MatchCase> &c : cases) {
      if (get_default() != c.get()) {
        case_ptrs.push_back(c.get());
      }
    }
    return case_ptrs;
  }

  /// Returns the default match case of this match statement.
  inline MatchCase* get_default() const {
    return cases.back().get();
  }
};


/// This class represents a function return statement.
class ReturnStmt final : public Stmt
{
private:
  std::unique_ptr<Expr> expr;

public:
  ReturnStmt(std::unique_ptr<Expr> expr, const Metadata &meta) : Stmt(meta), expr(std::move(expr)) {};
  void pass(ASTVisitor *visitor) override { visitor->visit(this); }

  /// Returns the expression of this return statement.
  inline Expr* get_expr() const { return expr.get(); }

  /// Returns true if this return statement has an expression, and false otherwise.
  inline bool has_expr() const { return expr != nullptr; }
  
  /// Returns a string representation of this AST node.
  const std::string to_string() override;
};


/// This class represents a looping until statement.
class UntilStmt final : public Stmt
{
private:
  std::unique_ptr<Expr> cond;
  std::unique_ptr<Stmt> body;

public:
  UntilStmt(std::unique_ptr<Expr> cond, std::unique_ptr<Stmt> body, const Metadata &meta)
    : Stmt(meta), cond(std::move(cond)), body(std::move(body)) {};
  void pass(ASTVisitor *visitor) override { visitor->visit(this); }

  /// Returns the condition expression of this until statement.
  inline Expr* get_cond() const { return cond.get(); }

  /// Returns the body of this until statement.
  inline Stmt* get_body() const { return body.get(); }

  /// Returns a string representation of this AST node.
  const std::string to_string() override;
};


/// BreakStmt - Represents a break statement.
///
/// Break statements are used to exit a loop statement prematurely.
class BreakStmt final : public Stmt
{
public:
  BreakStmt(const Metadata &meta) : Stmt(meta) {};
  void pass(ASTVisitor *visitor) override { visitor->visit(this); }

  /// Returns a string representation of this AST node.
  const std::string to_string() override;
};


/// ContinueStmt - Represents a continue statement.
///
/// Continue statements are used to skip the current iteration of a loop statement.
class ContinueStmt final : public Stmt
{

public:
  ContinueStmt(const Metadata &meta) : Stmt(meta) {};
  void pass(ASTVisitor *visitor) override { visitor->visit(this); }

  /// Returns a string representation of this AST node.
  const std::string to_string() override;
};

#endif  // STMT_STATIMC_H
