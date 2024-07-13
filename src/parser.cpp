// Copyright 2024 Nick Marino (github.com/nwmarino)

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
#include "llvm/Support/raw_ostream.h"

#include <string>
#include <memory>
#include <utility>

#include "parser.h"
#include "codegen.h"
#include "ast.h"
#include "token.h"
#include "logger.h"
#include "tstream.h"
#include <iostream>

using namespace llvm;

std::unique_ptr<Expr> parsePrimary(std::shared_ptr<tstream> cc);
std::unique_ptr<Expr>
parseBinOp(std::shared_ptr<tstream> cc, int basePrecedence, std::unique_ptr<Expr> leftSide);
std::unique_ptr<Expr> parseExpr(std::shared_ptr<tstream> cc);
int getPrecedence(std::shared_ptr<tstream> cc);
std::unique_ptr<Expr> parseNumerical(std::shared_ptr<tstream> cc);
std::unique_ptr<Expr> parseParentheses(std::shared_ptr<tstream> cc);
std::unique_ptr<Expr> parseIdentifier(std::shared_ptr<tstream> cc);
std::unique_ptr<PrototypeAST> parsePrototype(std::shared_ptr<tstream> cc);
std::unique_ptr<FunctionAST> parseDefinition(std::shared_ptr<tstream> cc);
std::unique_ptr<FunctionAST> parseTopLevelDefinition(std::shared_ptr<tstream> cc);
std::unique_ptr<Statement> parseReturnStatement(std::shared_ptr<tstream> cc);
std::unique_ptr<Statement> parseCompoundStatement(std::shared_ptr<tstream> cc);
std::unique_ptr<Statement> parseCompoundStatement(std::shared_ptr<tstream> cc);




std::unique_ptr<Expr> parsePrimary(std::shared_ptr<tstream> cc) {
  switch (cc->curr.type) {
    case Identifier:
      return parseIdentifier(cc);
    case Integer:
    case Float:
      return parseNumerical(cc);
    case SetParen:
      return parseParentheses(cc);
    default:
      return logError("Unable to resolve token of type " + std::to_string(cc->curr.type));
  }
}


std::unique_ptr<Expr>
parseBinOp(std::shared_ptr<tstream> cc, int basePrecedence, std::unique_ptr<Expr> leftSide)
{
  while (true) {
    int tokenPrecedence = getPrecedence(cc);

    if (tokenPrecedence < basePrecedence)
      return leftSide;

    int op = cc->curr.type;
    cc->next();
    auto rightSide = parsePrimary(cc);
    if (!rightSide)
      return nullptr;

    int nextPrecedence = getPrecedence(cc);
    if (tokenPrecedence < nextPrecedence) {
      rightSide = parseBinOp(cc, tokenPrecedence + 1, std::move(rightSide));
      if (!rightSide)
        return nullptr;
    }

    leftSide = std::make_unique<BinaryExpr>(op, std::move(leftSide), std::move(rightSide));
  }
}


/**
 * Parse an arbitrary, possibly compound expression.
 * 
 * @param cc Token stream.
 * @return   Pointer to an expression node.
 */
std::unique_ptr<Expr> parseExpr(std::shared_ptr<tstream> cc)
{
  if (auto base = parsePrimary(cc))
    return parseBinOp(cc, 0, std::move(base));
  return nullptr;
}


/**
 * TODO: Get precedence for the next token.
 * 
 * @param cc Token stream.
 * @return   Relative precendence for future token.
 */
int getPrecedence(std::shared_ptr<tstream> cc)
{
  return -1;
}


/**
 * Parse a numeric literal expression.
 * 
 * @param cc Token stream.
 * @return   Pointer to an expression node.
 */
std::unique_ptr<Expr> parseNumerical(std::shared_ptr<tstream> cc)
{
  auto node = std::make_unique<NumericalExpr>(stod(cc->curr.value));
  cc->next(); // eat literal
  return node;
}


/**
 * Parse an argument expression.
 * 
 * @param cc Token stream.
 * @return   Pointer to an expression node.
 */
std::unique_ptr<Expr> parseParentheses(std::shared_ptr<tstream> cc)
{
  cc->next(); // eat open parentheses

  auto exp = parseExpr(cc);
  if (!exp)
    return nullptr;

  if (cc->curr.type != EndParen)
    return logError("Unmatched opening parentheses.");

  cc->next(); // eat close parentheses
  return exp;
}


/**
 * Parse an identifier.
 * 
 * @param cc Token stream.
 * @return   Pointer to an expression node.
 */
std::unique_ptr<Expr> parseIdentifier(std::shared_ptr<tstream> cc)
{
  std::string id = cc->curr.value;

  cc->next(); // eat identifier
  
  if (cc->curr.type != SetParen)
    return std::make_unique<VariableExpr>(id);

  cc->next(); // eat open parentheses
  std::vector<std::unique_ptr<Expr>> args;
  while (cc->curr.type != EndParen) {
    if (auto arg = parseExpr(cc))
      args.push_back(std::move(arg));

    if (cc->curr.type != Comma)
      return logError("Expected separator between function call arguments.");

    cc->next(); // eat comma separator
  }
  cc->next(); // eat close parentheses
  return std::make_unique<FunctionCallExpr>(id, std::move(args));
}


/**
 * Parse a function prototype.
 * 
 * @param cc Token stream.
 * @return   Pointer to a prototype node.
 */
std::unique_ptr<PrototypeAST> parsePrototype(std::shared_ptr<tstream> cc)
{
  if (cc->curr.type != Identifier) 
    return logErrorPr("Function name must proceed definition.");

  std::string name = cc->curr.value;
  cc->next(); // eat identifier

  if (cc->curr.type != SetParen)
    return logErrorPr("Expected opening parentheses proceeding function identifier.");

  cc->next(); // eat open parentheses

  std::vector<std::string> argNames;
  while (cc->curr.type == Identifier) {
    argNames.push_back(cc->curr.value);
    cc->next(); // eat current identifier
  }

  if (cc->curr.type != EndParen)
    return logErrorPr("Expected closing parentheses proceeding function arguments.");

  cc->next(); // eat close parentheses
  cc->next(); // eat arrow

  RetType retType;
  switch (cc->curr.type) {
    case IntKeyword: retType = RT_INT; break;
    case FloatKeyword: retType = RT_FLOAT; break;
    case BoolKeyword: retType = RT_BOOL; break;
    case CharKeyword: retType = RT_CHAR; break;
    case StringKeyword: retType = RT_STRING; break;
    default:
      return logErrorPr("Unresolved return type");
  }

  cc->next(); // eat return type keyword
  return std::make_unique<PrototypeAST>(name, std::move(argNames), retType);
}


/**
 * Parse a function definition. Evaluates the prototype and body of a function definition.
 * 
 * @param cc Token stream.
 * @return   Pointer to a function node representing the definition.
 */
std::unique_ptr<FunctionAST> parseDefinition(std::shared_ptr<tstream> cc)
{
  cc->next(); // eat function keyword

  auto head = parsePrototype(cc);
  cc->next(); // eat opening block
  if (!head)
    return nullptr;

  if (auto body = parseCompoundStatement(cc))
    return std::make_unique<FunctionAST>(std::move(head), std::move(body));
  return nullptr;
}


/**
 * Parse a top-level definition.
 * 
 * @param cc Token stream.
 * @return   Pointer to a function node representing the definition.
 */
std::unique_ptr<FunctionAST> parseTopLevelDefinition(std::shared_ptr<tstream> cc)
{
  if (auto body = parseCompoundStatement(cc)) {
    auto head = std::make_unique<PrototypeAST>("", std::vector<std::string>(), RT_VOID);
    return std::make_unique<FunctionAST>(std::move(head), std::move(body));
  }
  return nullptr;
}


/**
 * Parse a return statement.
 * 
 * Evaluates the expression led by a return keyword and wraps it in a node.
 * 
 * @param cc Token stream.
 * @return   Pointer to return statement node.
 */
std::unique_ptr<Statement> parseReturnStatement(std::shared_ptr<tstream> cc)
{
  cc->next(); // eat return keyword

  if (auto exp = parseExpr(cc)) {
    if (cc->curr.type != Semicolon)
      return logErrorS("Expected semicolon.");
    cc->next(); // eat semicolon
    return std::make_unique<ReturnStatement>(std::move(exp));
  }

  return nullptr;
}


/**
 * Parse a compound statement.
 * 
 * Creates a special statement node which often represents a function body.
 * 
 * @param cc Token stream.
 * @return   Pointer to compound statement node.
 */
std::unique_ptr<Statement> parseCompoundStatement(std::shared_ptr<tstream> cc)
{
  // support actual function bodies later
  std::unique_ptr<Statement> stmt = parseReturnStatement(cc);

  if (cc->curr.type != EndBrace)
    return logErrorS("Expected block end.");
  
  cc->next(); // eat end block
  return stmt;
}


void HandleDefinition(std::shared_ptr<tstream> cc) {
  if (auto FnAST = parseDefinition(cc)) {
    if (auto *FnIR = FnAST->codegen()) {
      fprintf(stderr, "Parsed function definition:\n");
      FnIR->print(errs());
      fprintf(stderr, "\n");
      FnIR->eraseFromParent();
    }
  } else {
    cc->next();
  }
}

void HandleTopLevelExpression(std::shared_ptr<tstream> cc) {
  if (auto FnAST = parseTopLevelDefinition(cc)) {
    if (auto *FnIR = FnAST->codegen()) {
      fprintf(stderr, "Parsed top-level expression:\n");
      FnIR->print(errs());
      fprintf(stderr, "\n");
      FnIR->eraseFromParent();
    }
  } else {
    cc->next();
  }
}

void parse(std::shared_ptr<tstream> cc) {
  while (true) {
    switch (cc->curr.type) {
    case Terminate:
      return;
    case Semicolon:
      cc->next();
      break;
    case FunctionKeyword:
      HandleDefinition(cc);
      break;
    default:
      HandleTopLevelExpression(cc);
      break;
    }
  }
}