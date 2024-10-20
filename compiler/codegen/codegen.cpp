#include <string>
#include <map>

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Verifier.h"

#include "../include/ast/Expr.h"
#include "../include/ast/Decl.h"
#include "../include/ast/Stmt.h"
#include "../include/ast/Unit.h"
#include "../include/core/Logger.h"
#include "../include/core/Type.h"
#include "../include/sema/ASTVisitor.h"

static std::unique_ptr<llvm::Module> module = nullptr;
static std::unique_ptr<llvm::IRBuilder<>> builder = nullptr;
static std::unique_ptr<llvm::LLVMContext> llvm_ctx = nullptr;
static std::map<std::string, llvm::Value *> named_values = {};


/// This function takes in a statim type and returns the corresponding llvm type.
/// @param T  The statim type.
/// @return   The corresponding llvm type.
static inline llvm::Type *bridge_type(const Type *T) {
  if (T->is_bool()) {
    return llvm::Type::getInt1Ty(*llvm_ctx);
  } else if (T->is_void()) {
    return llvm::Type::getVoidTy(*llvm_ctx);
  } else if (T->is_integer()) {
    return llvm::Type::getInt64Ty(*llvm_ctx);
  } else if (T->is_float()) {
    return llvm::Type::getFloatTy(*llvm_ctx);
  } else {
    return nullptr;
  }
}


/* Declaration Codegen */

llvm::Function *FunctionDecl::codegen() const {
  std::vector<llvm::Type *> param_types(params.size());
  for (int i = 0; i < params.size(); i++) {
    param_types[i] = bridge_type(params[i]->get_type());
  }

  llvm::FunctionType *fn_type = llvm::FunctionType::get(bridge_type(T), param_types, false);
  llvm::Function *fn = llvm::Function::Create(fn_type, llvm::Function::ExternalLinkage, name, module.get());

  unsigned int i = 0;
  for (llvm::Argument &arg : fn->args()) {
    arg.setName(params[i++]->get_name());
  }

  if (!fn->empty()) {
    return (llvm::Function *) warn_cg_value("function redefined: " + name, meta);
  }

  llvm::BasicBlock *BB = llvm::BasicBlock::Create(*llvm_ctx, "entry", fn);
  builder->SetInsertPoint(BB);

  named_values.clear();
  for (llvm::Argument &arg : fn->args()) {
    named_values[std::string(arg.getName())] = &arg;
  }

  if (llvm::Value *ret_val = body->codegen()) {
    builder->CreateRet(ret_val);
    llvm::verifyFunction(*fn);
    return fn;
  }

  fn->eraseFromParent();
  return nullptr;
}


/* Statement Codegen */

llvm::Value *DeclStmt::codegen() const {}


llvm::Value *CompoundStmt::codegen() const {}


llvm::Value *IfStmt::codegen() const {}


llvm::Value *MatchCase::codegen() const {}


llvm::Value *MatchStmt::codegen() const {}


llvm::Value *ReturnStmt::codegen() const {}


llvm::Value *UntilStmt::codegen() const {}


llvm::Value *BreakStmt::codegen() const {}


llvm::Value *ContinueStmt::codegen() const {}


/* Expression Codegen */

llvm::Value *NullExpr::codegen() const {
  return llvm::Constant::getNullValue(bridge_type(T));
}


// wait for match
llvm::Value *DefaultExpr::codegen() const {}


llvm::Value *BooleanLiteral::codegen() const {
  return llvm::ConstantInt::get(*llvm_ctx, llvm::APInt(1, value, false));
}


llvm::Value *IntegerLiteral::codegen() const {
  return llvm::ConstantInt::get(*llvm_ctx, llvm::APInt(64, value, signedness));
}


llvm::Value *FPLiteral::codegen() const {
  return llvm::ConstantFP::get(*llvm_ctx, llvm::APFloat(value));
}


llvm::Value *CharLiteral::codegen() const {
  return llvm::ConstantInt::get(*llvm_ctx, llvm::APInt(8, value, false));
}


llvm::Value *StringLiteral::codegen() const {
  return builder->CreateGlobalStringPtr(value);
}


// scoping
llvm::Value *DeclRefExpr::codegen() const {
  if (llvm::Value *val = named_values[ident]) {
    return val;
  }
  return warn_cg_value("unresolved variable reference: " + ident, meta);
}


// needs all operator support
llvm::Value *BinaryExpr::codegen() const {
  llvm::Value *l_val = lhs->codegen();
  llvm::Value *r_val = rhs->codegen();

  if (!l_val || !r_val) {
    return nullptr;
  }

  switch (op) {
    case BinaryOp::Plus:
      return builder->CreateAdd(l_val, r_val, "addtmp");
    case BinaryOp::Minus:
      return builder->CreateSub(l_val, r_val, "subtmp");
    case BinaryOp::Mult:
      return builder->CreateMul(l_val, r_val, "multmp");
    case BinaryOp::Div:
      return builder->CreateSDiv(l_val, r_val, "divtmp");
    default:
      return warn_cg_value("unresolved binary operator: " + binary_to_string(op), meta);
  }
}


// needs all operator support
llvm::Value *UnaryExpr::codegen() const {
  llvm::Value *val = expr->codegen();

  if (!val) {
    return nullptr;
  }

  switch (op) {
    case UnaryOp::Bang:
      return builder->CreateNot(val, "nottmp");
    default:
      return warn_cg_value("unresolved unary operator: " + unary_to_string(op), meta);
  }
}


// scoping on target struct
llvm::Value *InitExpr::codegen() const {}


llvm::Value *CallExpr::codegen() const {
  llvm::Function *fn = module->getFunction(callee);

  if (!fn) {
    return warn_cg_value("unresolved function call: " + callee, meta);
  }

  if (fn->arg_size() != args.size()) {
    return warn_cg_value( callee + " expected " + std::to_string(fn->arg_size()) + " arguments but " + std::to_string(args.size()) + " were provided", meta);
  }

  std::vector<llvm::Value *> arg_vals;
  for (unsigned int i = 0, e = args.size(); i != e; ++i) {
    arg_vals.push_back(args[i]->codegen());
    if (!arg_vals.back()) {
      return nullptr;
    }
  }

  return builder->CreateCall(fn, arg_vals, "calltmp");
}


// scoping on base
llvm::Value *MemberExpr::codegen() const {}


// scoping on base
llvm::Value *MemberCallExpr::codegen() const {}


// handle
llvm::Value *ThisExpr::codegen() const {
  return named_values["this"];
}

