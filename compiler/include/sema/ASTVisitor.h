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
class VarDecl;

class DeclStmt;
class CompoundStmt;
class IfStmt;
class MatchCase;
class MatchStmt;
class WhileStmt;
class ForStmt;
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
  virtual void visit(VarDecl *d) = 0;
  
};

#endif  // VISITOR_STATIMC_H
