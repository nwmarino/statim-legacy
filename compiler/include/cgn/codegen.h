#ifndef CODEGEN_STATIMC_H
#define CODEGEN_STATIMC_H

#include <map>
#include <memory>
#include <string>

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Verifier.h"
#include <llvm/Target/TargetMachine.h>

#include "../core/ASTVisitor.h"

namespace cgn {

class Codegen : public ASTVisitor {
private:
  std::unique_ptr<llvm::LLVMContext> ctx;
  std::unique_ptr<llvm::Module> module;
  std::unique_ptr<llvm::IRBuilder<>> builder;
  std::map<std::string, llvm::Function *> fns;
  std::map<std::string, llvm::Value *> allocas;

  llvm::TargetMachine *tm;
  llvm::Function *parent_fn = nullptr;
  llvm::BasicBlock *curr_bb = nullptr;
  llvm::Value *temp_val = nullptr;

  void set_curr_bb(llvm::BasicBlock *bb) { 
    curr_bb = bb; 
    builder->SetInsertPoint(bb); 
  }

  llvm::AllocaInst *create_entry_alloca(llvm::Function *fn, const std::string &var, llvm::Type *ty);

  template<typename T> void codegen(const T e) { e->pass(this); }

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

public:
  Codegen(const std::string &pkg, llvm::TargetMachine *tm) : tm(tm) {
    ctx = std::make_unique<llvm::LLVMContext>();
    builder = std::make_unique<llvm::IRBuilder<>>(*ctx);
    module = std::make_unique<llvm::Module>(pkg, *ctx);
    builder = std::make_unique<llvm::IRBuilder<>>(*ctx);

    module->setTargetTriple(tm->getTargetTriple().getTriple());
    module->setDataLayout(tm->createDataLayout());
  }

  llvm::Module *get_module() { return module.get(); }
};

} // namespace cgn

#endif  // CODEGEN_STATIMC_H
