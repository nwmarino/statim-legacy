#include <string>
#include <map>

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"

#include "../include/ast/Unit.h"
#include "../include/ast/Decl.h"
#include "../include/ast/Expr.h"
#include "../include/ast/Stmt.h"
#include "../include/core/Logger.h"
#include "../include/sema/ASTVisitor.h"

static std::unique_ptr<llvm::Module> module = nullptr;
static std::unique_ptr<llvm::IRBuilder<>> builder = nullptr;
static std::unique_ptr<llvm::LLVMContext> llvm_ctx = nullptr;
static std::map<std::string, llvm::Value *> named_values = {};

void CodegenVisitor::visit(CrateUnit *u) {}
void CodegenVisitor::visit(PackageUnit *u) {}

void CodegenVisitor::visit(FunctionDecl *d) {}
void CodegenVisitor::visit(ParamVarDecl *d) {}
void CodegenVisitor::visit(StructDecl *d) {}
void CodegenVisitor::visit(FieldDecl *d) {}
void CodegenVisitor::visit(TraitDecl *d) {}
void CodegenVisitor::visit(ImplDecl *d) {}
void CodegenVisitor::visit(EnumDecl *d) {}
void CodegenVisitor::visit(EnumVariantDecl *d) {}
void CodegenVisitor::visit(VarDecl *d) {}

void CodegenVisitor::visit(DeclStmt *s) {}
void CodegenVisitor::visit(CompoundStmt *s) {}
void CodegenVisitor::visit(IfStmt *s) {}
void CodegenVisitor::visit(MatchCase *s) {}
void CodegenVisitor::visit(MatchStmt *s) {}
void CodegenVisitor::visit(UntilStmt *s) {}
void CodegenVisitor::visit(ReturnStmt *s) {}
void CodegenVisitor::visit(BreakStmt *s) {}
void CodegenVisitor::visit(ContinueStmt *s) {}

void CodegenVisitor::visit(NullExpr *e) {}
void CodegenVisitor::visit(DefaultExpr *e) {}
void CodegenVisitor::visit(BooleanLiteral *e) {}
void CodegenVisitor::visit(IntegerLiteral *e) {}
void CodegenVisitor::visit(FPLiteral *e) {}
void CodegenVisitor::visit(CharLiteral *e) {}
void CodegenVisitor::visit(StringLiteral *e) {}
void CodegenVisitor::visit(DeclRefExpr *e) {}
void CodegenVisitor::visit(BinaryExpr *e) {}
void CodegenVisitor::visit(UnaryExpr *e) {}
void CodegenVisitor::visit(InitExpr *e) {}
void CodegenVisitor::visit(CallExpr *e) {}
void CodegenVisitor::visit(MemberExpr *e) {}
void CodegenVisitor::visit(MemberCallExpr *e) {}
void CodegenVisitor::visit(ThisExpr *e) {}
