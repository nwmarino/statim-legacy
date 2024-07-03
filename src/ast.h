// Copyright (c) 2024 Nick Marino (github.com/nwmarino)

#ifndef AST_H
#define AST_H

class AST {
public:
  virtual ~AST() = default;
};


class Expr : public AST {
public:
  virtual ~Expr() = default;
};


class Statement : public AST {
public:
  virtual ~Statement() = default;
};


#endif  // AST_H