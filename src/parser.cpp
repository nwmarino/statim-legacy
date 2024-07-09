// Copyright (c) 2024 Nick Marino (github.com/nwmarino)

#include <string>
#include <memory>
#include <utility>

#include "parser.h"
#include "ast.h"
#include "token.h"
#include "logger.h"
#include "tstream.h"

std::unique_ptr<Expr> parseExpr(tstream cc) {
  auto lhs = parsePrimary(cc);

  if (!lhs)
    return nullptr;

  return parseBinOp(cc, 0, std::move(lhs));
}

std::unique_ptr<Expr> parseBinOp(tstream cc, int expPrec,
                                        std::unique_ptr<Expr> lhs) {
  while (true) {
    int tokenPrec = getPrecedence(cc);

    if (tokenPrec < expPrec)
      return lhs;

    int op = cc.curr().type;
    cc.next();
    auto rhs = parsePrimary(cc);
    if (!rhs)
      return nullptr;

    int nextPrec = getPrecedence(cc);
    if (tokenPrec < nextPrec) {
      rhs = parseBinOp(cc, tokenPrec + 1, std::move(rhs));
      if (!rhs)
        return nullptr;
    }

    lhs = std::make_unique<Binary>(op, std::move(lhs), std::move(rhs));
  }
}

int getPrecedence(tstream cc) {
  return -1;
}

std::unique_ptr<Expr> parseNum(tstream cc) {
  auto node = std::make_unique<Numerical>(stod(cc.curr().value));
  cc.next();
  return node;
}

std::unique_ptr<Expr> parseParentheses(tstream cc) {
  cc.next(); // discard '('

  auto exp = parseExpr(cc);
  if (!exp) {
    return nullptr;
  }

  if (cc.curr().type != EndParen) {
    return logError("expected ')'");
  }

  cc.next();
  return exp;
}

std::unique_ptr<Expr> parseIdentifier(tstream cc) {
  std::string id = cc.curr().value;

  if (cc.next().type != SetParen) {
    return std::make_unique<Variable>(id);
  }

  std::vector<std::unique_ptr<Expr>> args;
  if (cc.next().type != EndParen) {
    while (true) {
      if (auto arg = parseExpr(cc)) {
        args.push_back(std::move(arg));
      }

      if (cc.curr().type == EndParen) {
        break;
      }

      if (cc.curr().type != Comma) {
        logError("expected separator ','");
      }

      cc.next();
    }
  }

  cc.next();
  return std::make_unique<FunctionCall>(id, std::move(args));
}

std::unique_ptr<Expr> parsePrimary(tstream cc) {
  switch (cc.curr().type) {
    case Identifier:
      return parseIdentifier(cc);
    case Integer:
    case Float:
      return parseNum(cc);
    case SetParen:
      return parseParentheses(cc);
    default:
      logError("unable to resolve token of type" + std::to_string(cc.curr().type));
  }
}

std::unique_ptr<FunctionPrototype> parsePrototype(tstream cc) {
  if (cc.curr().type != Identifier) 
    return logErrorPr("Expected function name");

  std::string name = cc.curr().value;
  cc.next(); // eat identifier

  if (cc.curr().type != SetParen)
    return logErrorPr("Expected '('");

  cc.next(); // eat parentheses
  std::vector<std::string> argNames;
  while (cc.curr().type == Identifier) {
    argNames.push_back(cc.curr().value);
    cc.next(); // eat current name
  }

  if (cc.curr().type != EndParen)
    return logErrorPr("Expected ')");

  cc.next(); // eat parentheses
  cc.next(); // eat arrow
  cc.next(); // eat ret type

  return std::make_unique<FunctionPrototype>(name, std::move(argNames));
}

std::unique_ptr<Function> parseDefinition(tstream cc) {
  cc.next(); // eat "fn"

  auto head = parsePrototype(cc);
  if (!head)
    return nullptr;

  if (auto exp = parseExpr(cc))
    return std::make_unique<Function>(std::move(head), std::move(exp));
  return nullptr;
}

std::unique_ptr<Function> parseTopLevelDef(tstream cc) {
  if (auto exp = parseExpr(cc)) {
    auto head = std::make_unique<FunctionPrototype>("", std::vector<std::string>());
    return std::make_unique<Function>(std::move(head), std::move(exp));
  }
  return nullptr;
}

void handleDef(tstream cc) {
  if (parseDefinition(cc)) {
    std::cout << "Parsed a definition\n";
    std::cout << "Curr: " << std::to_string(cc.curr().type) << '\n';
  } else {
    cc.next();
  }
}

void handleExpr(tstream cc) {
  if (parseTopLevelDef(cc)) {
    std::cout << "Parsed a top-level definition\n";
    std::cout << "Curr: " << std::to_string(cc.curr().type) << '\n';
  } else {
    cc.next();
  }
}

void parse(tstream cc) {
  while (true) {
    switch (cc.curr().type) {
      case Terminate:
        return;
      case Semicolon:
        break;
      case FunctionKeyword:
        handleDef(cc);
        break;
      default:
        handleExpr(cc);
        break;
    }
    cc.next();
  }
}
