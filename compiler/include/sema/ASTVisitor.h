#ifndef VISITOR_STATIMC_H
#define VISITOR_STATIMC_H

/// Visitor pattern for AST passes.
/// Copyright 2024 Nick Marino (github.com/nwmarino)

class Decl;
class Expr;
class Unit;
class Stmt;

class FunctionDecl;
class ParamVarDecl;
class StructDecl;
class FieldDecl;
class TraitDecl;
class ImplDecl;
class EnumDecl;
class EnumVariant;
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
class ArrayAccessExpr;

class ASTVisitor
{
public:
  virtual ~ASTVisitor() = default;

  virtual void visit(FunctionDecl *d) = 0;
  virtual void visit(ParamVarDecl *d) = 0;
  virtual void visit(StructDecl *d) = 0;
  virtual void visit(FieldDecl *d) = 0;
  virtual void visit(TraitDecl *d) = 0;
  virtual void visit(ImplDecl *d) = 0;
  virtual void visit(EnumDecl *d) = 0;
  virtual void visit(EnumVariant *d) = 0;
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
  virtual void visit(ArrayAccessExpr *e) = 0;
};

#endif  // VISITOR_STATIMC_H
