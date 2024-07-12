// Copyright (c) 2024 Nick Marino (github.com/nwmarino)

#ifndef AST_H
#define AST_H

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IR/Value.h"

#include <string>
#include <vector>
#include <memory>
#include <utility>

typedef
enum {
  RT_VOID,
  RT_INT,
  RT_FLOAT,
  RT_BOOL,
  RT_CHAR,
  RT_STRING
} RetType;

class AST {
  public:
    virtual ~AST() = default;
    virtual llvm::Value *codegen() = 0;
};

class Expr : public AST {
  public:
    virtual ~Expr() = default;
    virtual llvm::Value *codegen() = 0;
};

class NumericalExpr : public Expr {
  double value;

  public:
    NumericalExpr(double value) : value(value) {}
    llvm::Value *codegen() override;
};

class VariableExpr : public Expr {
  std::string name;

  public:
    VariableExpr(const std::string &name) : name(name) {}
    llvm::Value *codegen() override;
};

class BinaryExpr : public Expr {
  char op;
  std::unique_ptr<Expr> leftSide, rightSide;

  public:
    BinaryExpr(char op, std::unique_ptr<Expr> leftSide,
      std::unique_ptr<Expr> rightSide)
      : op(op), leftSide(std::move(leftSide)), rightSide(std::move(rightSide)) {}
    llvm::Value *codegen() override;
};

class FunctionCallExpr : public Expr {
  std::string callee;
  std::vector<std::unique_ptr<Expr>> args;

  public:
    FunctionCallExpr(const std::string &callee, std::vector<std::unique_ptr<Expr>> args)
      : callee(callee), args(std::move(args)) {}
    llvm::Value *codegen() override;
};

class PrototypeAST : public AST {
  std::string name;
  std::vector<std::string> args;
  RetType retType;

  public:
    PrototypeAST(const std::string &name, std::vector<std::string> args, RetType retType)
      : name(name), args(std::move(args)), retType(retType) {}
    std::string getName() { return name; }
    RetType getRetType() { return retType; }
    llvm::Function *codegen() override;
};

class FunctionAST : public AST {
  std::unique_ptr<PrototypeAST> head;
  std::unique_ptr<Expr> body;

  public:
    FunctionAST(std::unique_ptr<PrototypeAST> head, std::unique_ptr<Expr> body)
      : head(std::move(head)), body(std::move(body)) {}
    llvm::Function *codegen() override;
};

#endif  // AST_H
