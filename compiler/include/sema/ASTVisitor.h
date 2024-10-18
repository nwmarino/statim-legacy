#ifndef ASTVISITOR_STATIMC_H
#define ASTVISITOR_STATIMC_H

/// Visitor pattern for AST passes.
/// Copyright 2024 Nick Marino (github.com/nwmarino)

class Decl;
class Expr;
class Unit;
class Stmt;

class CrateUnit;
class PackageUnit;

class FunctionDecl;
class ParamVarDecl;
class StructDecl;
class FieldDecl;
class TraitDecl;
class ImplDecl;
class EnumDecl;
class EnumVariantDecl;
class VarDecl;

class DeclStmt;
class CompoundStmt;
class IfStmt;
class MatchCase;
class MatchStmt;
class UntilStmt;
class ReturnStmt;
class BreakStmt;
class ContinueStmt;

class NullExpr;
class DefaultExpr;
class BooleanLiteral;
class IntegerLiteral;
class FPLiteral;
class CharLiteral;
class StringLiteral;
class DeclRefExpr;
class BinaryExpr;
class UnaryExpr;
class InitExpr;
class CallExpr;
class MemberExpr;
class MemberCallExpr;
class ThisExpr;

/// ASTVisitor - Base class to all ASt visitors.
///
/// This class implements a list of virtual methods that can be overridden by
/// subclasses to visit different AST nodes. Such visitors are used for AST
/// passes and subsequent code generation or lowering of the AST.
class ASTVisitor
{
public:
  virtual ~ASTVisitor() = default;

  virtual void visit(CrateUnit *u) = 0;
  virtual void visit(PackageUnit *u) = 0;

  virtual void visit(FunctionDecl *d) = 0;
  virtual void visit(ParamVarDecl *d) = 0;
  virtual void visit(StructDecl *d) = 0;
  virtual void visit(FieldDecl *d) = 0;
  virtual void visit(TraitDecl *d) = 0;
  virtual void visit(ImplDecl *d) = 0;
  virtual void visit(EnumDecl *d) = 0;
  virtual void visit(EnumVariantDecl *d) = 0;
  virtual void visit(VarDecl *d) = 0;

  virtual void visit(DeclStmt *s) = 0;
  virtual void visit(CompoundStmt *s) = 0;
  virtual void visit(IfStmt *s) = 0;
  virtual void visit(MatchCase *s) = 0;
  virtual void visit(MatchStmt *s) = 0;
  virtual void visit(UntilStmt *s) = 0;
  virtual void visit(ReturnStmt *s) = 0;
  virtual void visit(BreakStmt *s) = 0;
  virtual void visit(ContinueStmt *s) = 0;

  virtual void visit(NullExpr *e) = 0;
  virtual void visit(DefaultExpr *e) = 0;
  virtual void visit(BooleanLiteral *e) = 0;
  virtual void visit(IntegerLiteral *e) = 0;
  virtual void visit(FPLiteral *e) = 0;
  virtual void visit(CharLiteral *e) = 0;
  virtual void visit(StringLiteral *e) = 0;
  virtual void visit(DeclRefExpr *e) = 0;
  virtual void visit(BinaryExpr *e) = 0;
  virtual void visit(UnaryExpr *e) = 0;
  virtual void visit(InitExpr *e) = 0;
  virtual void visit(CallExpr *e) = 0;
  virtual void visit(MemberExpr *e) = 0;
  virtual void visit(MemberCallExpr *e) = 0;
  virtual void visit(ThisExpr *e) = 0;
};


/// PassVisitor - Concrete visitor class for semantic analysis passes.
///
/// This class implements the ASTVisitor interface and provides a set of
/// methods to visit different AST nodes and enforce semantics on each node,
/// as well as final name resolution and type checking.
class PassVisitor final : public ASTVisitor
{
public:
  void visit(CrateUnit *u) override;
  void visit(PackageUnit *u) override;

  void visit(FunctionDecl *d) override;
  void visit(ParamVarDecl *d) override;
  void visit(StructDecl *d) override;
  void visit(FieldDecl *d) override;
  void visit(TraitDecl *d) override;
  void visit(ImplDecl *d) override;
  void visit(EnumDecl *d) override;
  void visit(EnumVariantDecl *d) override;
  void visit(VarDecl *d) override;

  void visit(DeclStmt *s) override;
  void visit(CompoundStmt *s) override;
  void visit(IfStmt *s) override;
  void visit(MatchCase *s) override;
  void visit(MatchStmt *s) override;
  void visit(UntilStmt *s) override;
  void visit(ReturnStmt *s) override;
  void visit(BreakStmt *s) override;
  void visit(ContinueStmt *s) override;

  void visit(NullExpr *e) override;
  void visit(DefaultExpr *e) override;
  void visit(BooleanLiteral *e) override;
  void visit(IntegerLiteral *e) override;
  void visit(FPLiteral *e) override;
  void visit(CharLiteral *e) override;
  void visit(StringLiteral *e) override;
  void visit(DeclRefExpr *e) override;
  void visit(BinaryExpr *e) override;
  void visit(UnaryExpr *e) override;
  void visit(InitExpr *e) override;
  void visit(CallExpr *e) override;
  void visit(MemberExpr *e) override;
  void visit(MemberCallExpr *e) override;
  void visit(ThisExpr *e) override;
};

#endif  // ASTVISITOR_STATIMC_H
