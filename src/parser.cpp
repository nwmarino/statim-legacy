// Copyright (c) 2024 Nick Marino (github.com/nwmarino)

#include <string>
#include <memory>
#include <utility>

#include "parser.h"
#include "ast.h"
#include "token.h"
#include "logger.h"
#include "tstream.h"

std::unique_ptr<Expr> parseExpr(std::shared_ptr<tstream> cc) {
  auto leftSide = parsePrimary(cc);

  if (!leftSide)
    return nullptr;

  return parseBinOp(cc, 0, std::move(leftSide));
}

std::unique_ptr<Expr> parseBinOp(std::shared_ptr<tstream> cc, int expectedPrecedence,
                                 std::unique_ptr<Expr> leftSide) {
  while (true) {
    int tokenPrecedence = getPrecedence(cc);

    if (tokenPrecedence < expectedPrecedence)
      return leftSide;

    int op = (*cc).curr.type;
    (*cc).next();
    auto rightSide = parsePrimary(cc);
    if (!rightSide)
      return nullptr;

    int nextPrecedence = getPrecedence(cc);
    if (tokenPrecedence < nextPrecedence) {
      rightSide = parseBinOp(cc, tokenPrecedence + 1, std::move(rightSide));
      if (!rightSide)
        return nullptr;
    }

    leftSide = std::make_unique<BinaryExpr>(op, std::move(leftSide),
                                           std::move(rightSide));
  }
}

int getPrecedence(std::shared_ptr<tstream> cc) {
  return -1;
}

std::unique_ptr<Expr> parseNum(std::shared_ptr<tstream> cc) {
  auto node = std::make_unique<NumericalExpr>(stod((*cc).curr.value));
  (*cc).next();
  return node;
}

std::unique_ptr<Expr> parseParentheses(std::shared_ptr<tstream> cc) {
  (*cc).next(); // discard '('

  auto exp = parseExpr(cc);
  if (!exp) {
    return nullptr;
  }

  if ((*cc).curr.type != EndParen) {
    return logError("expected ')'");
  }

  (*cc).next();
  return exp;
}

std::unique_ptr<Expr> parseIdentifier(std::shared_ptr<tstream> cc) {
  std::string id = (*cc).curr.value;
  (*cc).next();
  if ((*cc).curr.type != SetParen) {
    return std::make_unique<VariableExpr>(id);
  }

  (*cc).next();
  std::vector<std::unique_ptr<Expr>> args;
  if ((*cc).curr.type != EndParen) {
    while (true) {
      if (auto arg = parseExpr(cc)) {
        args.push_back(std::move(arg));
      }

      if ((*cc).curr.type == EndParen) {
        break;
      }

      if ((*cc).curr.type != Comma) {
        logError("expected separator ','");
      }

      (*cc).next();
    }
  }

  (*cc).next();
  return std::make_unique<FunctionCallExpr>(id, std::move(args));
}

std::unique_ptr<Expr> parsePrimary(std::shared_ptr<tstream> cc) {
  switch ((*cc).curr.type) {
    case Identifier:
      return parseIdentifier(cc);
    case Integer:
    case Float:
      return parseNum(cc);
    case SetParen:
      return parseParentheses(cc);
    default:
      logError("unable to resolve token of type" + std::to_string((*cc).curr.type));
  }
}

std::unique_ptr<PrototypeAST> parsePrototype(std::shared_ptr<tstream> cc) {
  if ((*cc).curr.type != Identifier) 
    return logErrorPr("Expected function name");

  std::string name = (*cc).curr.value;
  (*cc).next(); // eat identifier

  if ((*cc).curr.type != SetParen)
    return logErrorPr("Expected '('");

  (*cc).next(); // eat parentheses
  std::vector<std::string> argNames;
  while ((*cc).curr.type == Identifier) {
    argNames.push_back((*cc).curr.value);
    (*cc).next(); // eat current name
  }

  if ((*cc).curr.type != EndParen)
    return logErrorPr("Expected ')");

  (*cc).next(); // eat parentheses
  (*cc).next(); // eat arrow
  (*cc).next(); // eat ret type

  return std::make_unique<PrototypeAST>(name, std::move(argNames));
}

std::unique_ptr<FunctionAST> parseDefinition(std::shared_ptr<tstream> cc) {
  (*cc).next(); // eat "fn"

  auto head = parsePrototype(cc);
  if (!head)
    return nullptr;

  if (auto exp = parseExpr(cc))
    return std::make_unique<FunctionAST>(std::move(head), std::move(exp));
  return nullptr;
}

std::unique_ptr<FunctionAST> parseTopLevelDef(std::shared_ptr<tstream> cc) {
  if (auto exp = parseExpr(cc)) {
    auto head = std::make_unique<PrototypeAST>("", std::vector<std::string>());
    return std::make_unique<FunctionAST>(std::move(head), std::move(exp));
  }
  return nullptr;
}
