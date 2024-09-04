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
  
};

#endif  // VISITOR_STATIMC_H
