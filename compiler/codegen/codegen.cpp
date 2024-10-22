#include <string>
#include <map>

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Verifier.h"

#include "../include/ast/Expr.h"
#include "../include/ast/Decl.h"
#include "../include/ast/Stmt.h"
#include "../include/ast/Unit.h"
#include "../include/cgn/codegen.h"
#include "../include/core/ASTVisitor.h"
#include "../include/core/Logger.h"
#include "../include/core/Type.h"


static std::unique_ptr<llvm::Module> module = nullptr;
static std::unique_ptr<llvm::IRBuilder<>> builder = nullptr;
static std::unique_ptr<llvm::LLVMContext> llvm_ctx = nullptr;
static std::map<std::string, llvm::Value *> named_values = {};

namespace cgn {
  void init(void) {
    llvm_ctx = std::make_unique<llvm::LLVMContext>();
    module = std::make_unique<llvm::Module>("statimc", *llvm_ctx);
    builder = std::make_unique<llvm::IRBuilder<>>(*llvm_ctx);
  }

  void print_mod(void) {
    module->print(llvm::errs(), nullptr);
  }
}
