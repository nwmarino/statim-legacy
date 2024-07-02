// Copyright (c) 2024 Nick Marino (github.com/nwmarino)

#ifndef AST_H
#define AST_H

#include <string>
#include <vector>

using std::string;

typedef
enum {
  RT_VOID,
  RT_INT,
  RT_CHAR,
  RT_STRING,
  RT_FLOAT,
  RT_BOOL
} RetType;

class Ast {
  public:
    Ast();
};

class Program: public Ast {
  public:
    Program(Function *main);

  private:
    Function *main;
};

class Expression: public Ast {
  public:
    Expression();
};

class Constant: public Expression {
  public:
    Constant();
};

class Statement: public Ast {
  public:
    Statement();
};

class Return: public Statement {
  public:
    Return(Expression *value);
};

class Function: public Ast {
  public:
    Function(string name, Statement *body);

  private:
    string name;
    Statement *body;
    RetType retType;
};

#endif  // AST_H