// Copyright 2024 Nick Marino (github.com/nwmarino)

#ifndef AST_H
#define AST_H

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/LLVMContext.h"
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


/**
 * Base class to all other abstract syntax nodes.
 */
class AST {
  public:
    virtual ~AST() = default;
    virtual llvm::Value *codegen() = 0;
};


/**
 * Base class to expression nodes.
 */
class Expr : public AST {
  public:
    virtual ~Expr() = default;
    virtual llvm::Value *codegen() = 0;
};


/**
 * Base class to statement nodes.
 */
class Statement : public AST {
  public:
    virtual ~Statement() = default;
    virtual llvm::Value *codegen() = 0;
};


/**
 * Expression class for numeric integer literals.
 */
class NumericalExpr : public Expr {
  long value;

  public:
    NumericalExpr(long value) : value(value) {}
    llvm::Value *codegen() override;
};


/**
 * Expression class for variables.
 */
class VariableExpr : public Expr {
  std::string name;

  public:
    VariableExpr(const std::string &name) : name(name) {}
    llvm::Value *codegen() override;
};


/**
 * Expression class for binary operations.
 */
class BinaryExpr : public Expr {
  char op;
  std::unique_ptr<Expr> leftSide, rightSide;

  public:
    BinaryExpr(char op, std::unique_ptr<Expr> leftSide,
      std::unique_ptr<Expr> rightSide)
      : op(op), leftSide(std::move(leftSide)), rightSide(std::move(rightSide)) {}
    llvm::Value *codegen() override;
};


/**
 * Expression class for function calls.
 */
class FunctionCallExpr : public Expr {
  std::string callee;
  std::vector<std::unique_ptr<Expr>> args;

  public:
    FunctionCallExpr(const std::string &callee, std::vector<std::unique_ptr<Expr>> args)
      : callee(callee), args(std::move(args)) {}
    llvm::Value *codegen() override;
};


/**
 * Node class for function prototypes.
 */
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


/**
 * Node class for function definitions.
 */
class FunctionAST : public AST {
  std::unique_ptr<PrototypeAST> head;
  std::unique_ptr<Statement> body;

  public:
    FunctionAST(std::unique_ptr<PrototypeAST> head, std::unique_ptr<Statement> body)
      : head(std::move(head)), body(std::move(body)) {}
    llvm::Function *codegen() override;
};


/**
 * Statement class for return statements.
 */
class ReturnStatement : public Statement {
  std::unique_ptr<Expr> expr;

  public:
    ReturnStatement(std::unique_ptr<Expr> expr) : expr(std::move(expr)) {}
    llvm::Value *codegen() override;
};


#endif  // AST_H
