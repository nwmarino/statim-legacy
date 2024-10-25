#include "../include/ast/Expr.h"
#include "../include/cgn/codegen.h"
#include "../include/core/Logger.h"
#include <iostream>
#include <llvm/IR/Verifier.h>

namespace cgn {

llvm::AllocaInst *Codegen::create_entry_alloca(llvm::Function *fn, const std::string &var, llvm::Type *ty) {
  llvm::IRBuilder<> temp_builder(&fn->getEntryBlock(), fn->getEntryBlock().begin());
  return temp_builder.CreateAlloca(ty, nullptr, var);
}

void Codegen::visit(CrateUnit *u) {}

void Codegen::visit(PackageUnit *u) {
  for (Decl * &decl : u->get_decls()) {
    // forward declare function declarations
    if (FunctionDecl *fn_decl = dynamic_cast<FunctionDecl *>(decl)) {
      // convert function types (return, arguments) to llvm equivelants
      llvm::Type *ret_type = fn_decl->get_type()->to_llvm_ty(*ctx);
      std::vector<llvm::Type *> arg_types;
      for (ParamVarDecl *arg : fn_decl->get_params())
        arg_types.push_back(arg->get_type()->to_llvm_ty(*ctx));

      // create function type, function
      llvm::FunctionType *fn_type = llvm::FunctionType::get(ret_type, arg_types, false);
      llvm::Function *fn = llvm::Function::Create(fn_type, llvm::Function::ExternalLinkage, fn_decl->get_name(), module.get());

      // set argument names
      unsigned idx = 0;
      for (llvm::Argument &arg : fn->args())
        arg.setName(fn_decl->get_param(idx++)->get_name());
      
      // store function in table
      fns[fn_decl->get_name()] = fn;
    } else
      codegen(decl);
  }

  for (Decl * &decl : u->get_decls()) {
    if (FunctionDecl *fn_decl = dynamic_cast<FunctionDecl *>(decl))
      codegen(fn_decl);
  }
}

/// Declaration Codegen

void Codegen::visit(FunctionDecl *d) {
  llvm::Function *fn = fns[d->get_name()];
  if (!fn)
    llvm_unreachable("undefined function");

  parent_fn = fn;
  llvm::BasicBlock *entry_bb = llvm::BasicBlock::Create(*ctx, "entry", fn);
  builder->SetInsertPoint(entry_bb);

  // create alloca for each function argument
  allocas.clear();
  for (llvm::Argument &arg : fn->args()) {
    llvm::AllocaInst *alloca = create_entry_alloca(fn, arg.getName().str(), arg.getType());
    builder->CreateStore(&arg, alloca);

    allocas[arg.getName().str()] = alloca;
  }
  codegen(d->get_body());
  llvm::verifyFunction(*fn);
}

void Codegen::visit(ParamVarDecl *d) {}
void Codegen::visit(StructDecl *d) {}
void Codegen::visit(FieldDecl *d) {}
void Codegen::visit(TraitDecl *d) {}
void Codegen::visit(ImplDecl *d) {}
void Codegen::visit(EnumDecl *d) {}
void Codegen::visit(EnumVariantDecl *d) {}

void Codegen::visit(VarDecl *d) {
  codegen(d->get_expr());
  llvm::AllocaInst *alloca = create_entry_alloca(parent_fn, d->get_name(), d->get_type()->to_llvm_ty(*ctx));
  builder->CreateStore(temp_val, alloca);

  allocas[d->get_name()] = alloca;
}

/// Statement Codegen

void Codegen::visit(DeclStmt *s) {
  codegen(s->get_decl());
}

void Codegen::visit(CompoundStmt *s) {
  for (Stmt * &stmt : s->get_stmts())
    codegen(stmt);
}

void Codegen::visit(IfStmt *s) {}
void Codegen::visit(MatchCase *s) {}
void Codegen::visit(MatchStmt *s) {}
void Codegen::visit(UntilStmt *s) {}

void Codegen::visit(ReturnStmt *s) {
  codegen(s->get_expr());
  
  builder->CreateRet(temp_val);
}

void Codegen::visit(BreakStmt *s) {}
void Codegen::visit(ContinueStmt *s) {}

/// Expression Codegen

void Codegen::visit(NullExpr *e) {}
void Codegen::visit(DefaultExpr *e) {}
void Codegen::visit(BooleanLiteral *e) {}

void Codegen::visit(IntegerLiteral *e) {
  // check if integer can be 32-bit
  temp_val = llvm::ConstantInt::get(*ctx, llvm::APInt(64, e->get_value(), true));
}

void Codegen::visit(FPLiteral *e) {}
void Codegen::visit(CharLiteral *e) {}
void Codegen::visit(StringLiteral *e) {}

void Codegen::visit(DeclRefExpr *e) {
  llvm::AllocaInst *alloca = allocas[e->get_ident()];
  if (!alloca)
    llvm_unreachable("undefined variable");

  temp_val = builder->CreateLoad(alloca->getAllocatedType(), alloca, e->get_ident().c_str());
}

void Codegen::visit(BinaryExpr *e) {
  llvm::Value *lhs = nullptr;
  llvm::Value *rhs = nullptr;

  codegen(e->get_lhs());
  lhs = temp_val;
  codegen(e->get_rhs());
  rhs = temp_val;

  switch (e->get_op()) {
    // lval saw check during sema pass, skip here, assignments valid by now
    case BinaryOp::Assign:
      builder->CreateStore(rhs, lhs);
      break;
    case BinaryOp::AddAssign:
      builder->CreateStore( builder->CreateAdd(lhs, rhs, "addtmp"), lhs);
      break;
    case BinaryOp::SubAssign:
      builder->CreateStore( builder->CreateSub(lhs, rhs, "subtmp"), lhs);
      break;
    case BinaryOp::StarAssign:
      builder->CreateStore(builder->CreateMul(lhs, rhs, "multmp"), lhs);
      break;
    case BinaryOp::SlashAssign:
      builder->CreateStore(builder->CreateSDiv(lhs, rhs, "divtmp"), lhs);
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
}

void Codegen::visit(UnaryExpr *e) {}
void Codegen::visit(InitExpr *e) {}

void Codegen::visit(CallExpr *e) {
  llvm::Function *callee = fns[e->get_callee()];
  if (!callee)
    llvm_unreachable("unresolved function call");

  if (callee->arg_size() != e->get_args().size())
    llvm_unreachable("incorrect number of arguments");

  std::vector<llvm::Value *> args;
  for (Expr *arg : e->get_args()) {
    codegen(arg);
    if (!temp_val)
      llvm_unreachable("invalid argument");

    args.push_back(temp_val);
  }

  temp_val = builder->CreateCall(callee, args, "calltmp");
}

void Codegen::visit(MemberExpr *e) {}
void Codegen::visit(MemberCallExpr *e) {}
void Codegen::visit(ThisExpr *e) {}

} // namespace cgn
