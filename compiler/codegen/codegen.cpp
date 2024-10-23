#include "../include/ast/Expr.h"
#include "../include/cgn/codegen.h"
#include "../include/core/Logger.h"

namespace cgn {

void Codegen::visit(CrateUnit *u) {}

void Codegen::visit(PackageUnit *u) {
  for (Decl *decl : u->get_decls()) {
    if (FunctionDecl *fn_decl = dynamic_cast<FunctionDecl *>(decl)) {
      llvm::Type *ret_type = fn_decl->get_type() ? fn_decl->get_type()->to_llvm_ty(*ctx) : llvm::Type::getVoidTy(*ctx);

      std::vector<llvm::Type *> arg_tys;
      for (ParamVarDecl *param : fn_decl->get_params()) {
        arg_tys.push_back(param->get_type()->to_llvm_ty(*ctx));
      }

      llvm::FunctionType *func_ty = llvm::FunctionType::get(ret_type, arg_tys, false);
      llvm::Function *func = llvm::Function::Create(func_ty, llvm::Function::ExternalLinkage, fn_decl->get_name(), module.get());

      vtable.insert({fn_decl->get_name(), func});
    } else {
      evalue(decl);
    }
  }

  for (Decl *decl : u->get_decls()) {
    evalue(decl);
  }
}

/// Declaration Codegen

void Codegen::visit(FunctionDecl *d) {
  llvm::Function *fn = llvm::dyn_cast<llvm::Function>(vtable.find(d->get_name())->second);
  parent_fn = fn;

  llvm::BasicBlock *entry_bb = llvm::BasicBlock::Create(*ctx, "entry", fn);
  set_curr_bb(entry_bb);

  llvm::Argument *llvm_arg = fn->args().begin();
  for (int i = 0; i < d->get_num_params(); i++) {
    llvm::IRBuilder<> temp_builder(&parent_fn->getEntryBlock(), parent_fn->getEntryBlock().begin());

    llvm::AllocaInst *alloca = temp_builder.CreateAlloca(llvm_arg->getType(), 0, d->get_param(i)->get_name());
    temp_builder.CreateStore(llvm_arg, alloca);
    vtable.insert({d->get_param(i)->get_name(), alloca});
  }
  evalue(d->get_body());

  // insert void return if function return type is void
  if (!d->get_type()) {
    builder->CreateRet(nullptr);
  }

  if (llvm::verifyFunction(*fn, &llvm::errs())) {
    panic("bad codegen");
  }
}

void Codegen::visit(ParamVarDecl *d) {
  llvm::IRBuilder<> temp_builder(&parent_fn->getEntryBlock(), parent_fn->getEntryBlock().begin());
  llvm::AllocaInst *alloca = temp_builder.CreateAlloca(d->get_type()->to_llvm_ty(*ctx), 0, d->get_name());
  vtable.insert({d->get_name(), alloca});
}

void Codegen::visit(StructDecl *d) {}
void Codegen::visit(FieldDecl *d) {}
void Codegen::visit(TraitDecl *d) {}
void Codegen::visit(ImplDecl *d) {}
void Codegen::visit(EnumDecl *d) {}
void Codegen::visit(EnumVariantDecl *d) {}

void Codegen::visit(VarDecl *d) {
  llvm::IRBuilder<> temp_builder(&parent_fn->getEntryBlock(), parent_fn->getEntryBlock().begin());
  llvm::AllocaInst *alloca = temp_builder.CreateAlloca(d->get_type()->to_llvm_ty(*ctx), 0, d->get_name());
  vtable.insert({d->get_name(), alloca});
}

/// Statement Codegen

void Codegen::visit(DeclStmt *s) {}

void Codegen::visit(CompoundStmt *s) {
  for (Stmt *stmt : s->get_stmts()) {
    evalue(stmt);
  }
}

void Codegen::visit(IfStmt *s) {
  evalue(s->get_cond());
  llvm::Value *cond = temp_val;

  llvm::BasicBlock *then_bb = llvm::BasicBlock::Create(*ctx, "then_bb", parent_fn);
  llvm::BasicBlock *merge_bb = llvm::BasicBlock::Create(*ctx, "merge_bb");
  llvm::BasicBlock *else_bb = s->get_else_body() ? merge_bb : llvm::BasicBlock::Create(*ctx, "else_bb");

  builder->CreateCondBr(cond, then_bb, else_bb);
  set_curr_bb(then_bb);

  builder->CreateBr(merge_bb);
  if (s->get_else_body()) {
    parent_fn->insert(parent_fn->end(), else_bb);
    set_curr_bb(else_bb);
    builder->CreateBr(merge_bb);
  }

  parent_fn->insert(parent_fn->end(), merge_bb);
  set_curr_bb(merge_bb);
}

void Codegen::visit(MatchCase *s) {}
void Codegen::visit(MatchStmt *s) {}
void Codegen::visit(UntilStmt *s) {}

void Codegen::visit(ReturnStmt *s) {
  evalue(s->get_expr());
  builder->CreateRet(temp_val);
}

void Codegen::visit(BreakStmt *s) {}
void Codegen::visit(ContinueStmt *s) {}

/// Expression Codegen

void Codegen::visit(NullExpr *e) {
  temp_val = llvm::Constant::getNullValue(e->get_type()->to_llvm_ty(*ctx));
}

void Codegen::visit(DefaultExpr *e) {}

void Codegen::visit(BooleanLiteral *e) {
  temp_val = llvm::ConstantInt::get(e->get_type()->to_llvm_ty(*ctx), 
    llvm::APInt(1, e->get_value(), false));
}

void Codegen::visit(IntegerLiteral *e) {
  temp_val = llvm::ConstantInt::get(e->get_type()->to_llvm_ty(*ctx), 
    llvm::APInt(64, e->get_value(), e->is_signed()));
}

void Codegen::visit(FPLiteral *e) {
  temp_val = llvm::ConstantFP::get(e->get_type()->to_llvm_ty(*ctx), e->get_value());
}

void Codegen::visit(CharLiteral *e) {
  temp_val = llvm::ConstantInt::get(e->get_type()->to_llvm_ty(*ctx), 
    llvm::APInt(8, e->get_value(), false));
}

void Codegen::visit(StringLiteral *e) {
  temp_val = builder->CreateGlobalStringPtr(e->get_value());
}

void Codegen::visit(DeclRefExpr *e) {
  llvm::Value *alloca = vtable.find(e->get_ident())->second;
  assert(alloca && "undefined alloca");

  temp_val = alloca;
}

void Codegen::visit(BinaryExpr *e) {
  llvm::Value *lhs = nullptr;
  llvm::Value *rhs = nullptr;

  evalue(e->get_lhs());
  lhs = temp_val;
  evalue(e->get_rhs());
  rhs = temp_val;

  switch (e->get_op()) {
    case BinaryOp::Assign:
      builder->CreateStore(lhs, rhs);
      break;
    case BinaryOp::AddAssign:
      builder->CreateStore(lhs, builder->CreateAdd(lhs, rhs, "addtmp"));
      break;
    case BinaryOp::SubAssign:
      builder->CreateStore(lhs, builder->CreateSub(lhs, rhs, "subtmp"));
      break;
    case BinaryOp::StarAssign:
      builder->CreateStore(lhs, builder->CreateMul(lhs, rhs, "multmp"));
      break;
    case BinaryOp::SlashAssign:
      builder->CreateStore(lhs, builder->CreateSDiv(lhs, rhs, "divtmp"));
      break;
    case BinaryOp::Plus:
      temp_val = builder->CreateAdd(lhs, rhs, "addtmp");
      break;
    case BinaryOp::Minus:
      temp_val = builder->CreateSub(lhs, rhs, "subtmp");
      break;
    case BinaryOp::Mult:
      temp_val = builder->CreateMul(lhs, rhs, "multmp");
      break;
    case BinaryOp::Div:
      temp_val = builder->CreateSDiv(lhs, rhs, "divtmp");
      break;
    case BinaryOp::IsEq:
      temp_val = builder->CreateICmpEQ(lhs, rhs, "eqtmp");
      break;
    case BinaryOp::IsNotEq:
      temp_val = builder->CreateICmpNE(lhs, rhs, "noteqtmp");
      break;
    case BinaryOp::LogicAnd:
      temp_val = builder->CreateAnd(lhs, rhs, "andtmp");
      break;
    case BinaryOp::LogicOr:
      temp_val = builder->CreateOr(lhs, rhs, "ortmp");
      break;
    case BinaryOp::Lt:
      temp_val = builder->CreateICmpSLT(lhs, rhs, "lttmp");
      break;
    case BinaryOp::LtEquals:
      temp_val = builder->CreateICmpSLE(lhs, rhs, "ltetmp");
      break;
    case BinaryOp::Gt:
      temp_val = builder->CreateICmpSGT(lhs, rhs, "gttmp");
      break;
    case BinaryOp::GtEquals:
      temp_val = builder->CreateICmpSGE(lhs, rhs, "gtetmp");
      break;
    default:
      llvm_unreachable("unexpected binary expression kind");
  }
  temp_val = nullptr;
}

void Codegen::visit(UnaryExpr *e) {
  evalue(e->get_expr());

  switch (e->get_op()) {
    case UnaryOp::Access:
      break;
    case UnaryOp::Bang:
      temp_val = builder->CreateNot(temp_val, "nottmp");
    case UnaryOp::Rune:
      break;
    case UnaryOp::Ref:
      break;
    default:
      llvm_unreachable("unexpected unary expression kind");
  }
}

void Codegen::visit(InitExpr *e) {}
  
void Codegen::visit(CallExpr *e) {
  llvm::Function *callee = llvm::dyn_cast<llvm::Function>(vtable.find(e->get_callee())->second);
  if (!callee) {
    panic("unresolved function call: " + e->get_callee(), e->get_meta());
  }

  std::vector<llvm::Value *> args;
  for (Expr *arg : e->get_args()) {
    evalue(arg);
    args.push_back(temp_val);
  }

  temp_val = builder->CreateCall(callee, args, "calltmp");
}

void Codegen::visit(MemberExpr *e) {}
void Codegen::visit(MemberCallExpr *e) {}
void Codegen::visit(ThisExpr *e) {}

} // namespace cgn
