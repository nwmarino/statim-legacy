// Copyright (c) 2024 Nick Marino (github.com/nwmarino)

#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <memory>
#include <utility>

class AST {
  public:
    virtual ~AST() = default;
};


class Expr : public AST {
  public:
    virtual ~Expr() = default;
};


class Numerical : public Expr {
  double value;

  public:
    Numerical(double value) : value(value) {}
};


class FunctionCall : public Expr {
  std::string callee;
  std::vector<std::unique_ptr<Expr>> args;

  public:
    FunctionCall(const std::string &callee, std::vector<std::unique_ptr<Expr>> args)
      : callee(callee), args(std::move(args)) {}
};


class Variable : public Expr {
  std::string name;

  public:
    Variable(const std::string &name) : name(name) {}
};


class Binary : public Expr {
  char op;
  std::unique_ptr<Expr> LHS, RHS;

  public:
    Binary(char op, std::unique_ptr<Expr> LHS, std::unique_ptr<Expr> RHS)
      : op(op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
};


class FunctionPrototype : public AST {
  std::string name;
  std::vector<std::string> args;

  public:
    FunctionPrototype(const std::string &name, std::vector<std::string> args)
      : name(name), args(std::move(args)) {}
};


class Function : public AST {
  std::unique_ptr<FunctionPrototype> head;
  std::unique_ptr<Expr> body;

  public:
    Function(std::unique_ptr<FunctionPrototype> head, std::unique_ptr<Expr> body)
      : head(std::move(head)), body(std::move(body)) {}
};

#endif  // AST_H