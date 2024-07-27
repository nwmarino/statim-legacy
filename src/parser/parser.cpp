/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <memory>
#include <string>
#include <utility>

#include "../include/ast.h"
#include "../include/logger.h"
#include "../include/parser.h"
#include "../include/token.h"
#include "../include/tstream.h"

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
std::unique_ptr<Statement> parse_compound_statement(std::shared_ptr<tstream> cc);


/**
 * Parse a primary expression.
 * 
 * @param cc Token stream.
 * @return   Pointer to an expression node.
 */
std::unique_ptr<Expr> parsePrimary(std::shared_ptr<tstream> cc) {
  switch (cc->curr.type) {
    case IDENTIFIER:
      return parseIdentifier(cc);
    case C_INT:
    case C_FP:
      return parseNumerical(cc);
    case SET_PAREN:
      return parseParentheses(cc);
    default:
      return logError("Unable to resolve token of type " + std::to_string(cc->curr.type));
  }
}


/**
 * Parse a binary operation.
 * 
 * @param cc             Token stream.
 * @param basePrecedence Precedence of the current operation.
 */
std::unique_ptr<Expr>
parseBinOp(std::shared_ptr<tstream> cc, int basePrecedence, std::unique_ptr<Expr> leftSide) {
  while (true) {
    int tokenPrecedence = getPrecedence(cc);

    if (tokenPrecedence < basePrecedence)
      return leftSide;

    int op = cc->curr.type;
    cc->next();
    std::unique_ptr<Expr> rightSide = parsePrimary(cc);
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
 * Parse an arbitrary expression.
 * 
 * @param cc Token stream.
 * @return   Pointer to an expression node.
 */
std::unique_ptr<Expr> parseExpr(std::shared_ptr<tstream> cc) {
  if (std::unique_ptr<Expr> base = parsePrimary(cc))
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
  TokenType numerType = cc->curr.type;
  std::string value = cc->curr.value;
  cc->next(); // eat literal

  if (numerType == C_INT)
    return std::make_unique<IntegerExpr>(stoll(value));
  else if (numerType == C_FP)
    return std::make_unique<FloatingPointExpr>(stod(value));
  else
    return nullptr;
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

  std::unique_ptr<Expr> exp = parseExpr(cc);
  if (!exp)
    return nullptr;

  if (cc->curr.type != END_PAREN)
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
  
  if (cc->curr.type != SET_PAREN)
    return std::make_unique<VariableExpr>(id);

  cc->next(); // eat open parentheses
  std::vector<std::unique_ptr<Expr>> args;
  while (cc->curr.type != END_PAREN) {
    if (std::unique_ptr<Expr> arg = parseExpr(cc))
      args.push_back(std::move(arg));

    if (cc->curr.type != COMMA)
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
  if (cc->curr.type != IDENTIFIER)
    return logErrorPr("Function name must proceed definition.");

  std::string name = cc->curr.value;
  cc->next(); // eat identifier

  if (cc->curr.type != SET_PAREN)
    return logErrorPr("Expected opening parentheses proceeding function identifier.");

  cc->next(); // eat open parentheses

  std::vector<std::string> argNames;
  while (cc->curr.type == IDENTIFIER) {
    argNames.push_back(cc->curr.value);
    cc->next(); // eat current identifier
  }

  if (cc->curr.type != END_PAREN)
    return logErrorPr("Expected closing parentheses proceeding function arguments.");

  cc->next(); // eat close parentheses
  cc->next(); // eat arrow

  RetType retType;
  switch (cc->curr.type) {
    case INT_KEYWORD: retType = RT_INT; break;
    case FLOAT_KEYWORD: retType = RT_FLOAT; break;
    case BOOL_KEYWORD: retType = RT_BOOL; break;
    case CHAR_KEYWORD: retType = RT_CHAR; break;
    case STRING_KEYWORD: retType = RT_STRING; break;
    default:
      return logErrorPr("Expected return type");
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

  std::unique_ptr<PrototypeAST> head = parsePrototype(cc);

  if (!head)
    return nullptr;

  cc->next(); // eat opening block
  if (std::unique_ptr<Statement> body = parse_compound_statement(cc))
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
  if (std::unique_ptr<Statement> body = parse_compound_statement(cc)) {
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

  if (std::unique_ptr<Expr> exp = parseExpr(cc)) {
    if (cc->curr.type != SEMICOLON)
      return logErrorS("Expected semicolon.");

    cc->next(); // eat semicolon
    return std::make_unique<ReturnStatement>(std::move(exp));
  }

  return nullptr;
}


/**
 * Parse a variable assignment.
 * 
 * @param cc Token stream.
 * @return   Pointer to an assignment node.
 */
std::unique_ptr<Statement> parse_declaration(std::shared_ptr<tstream> cc)
{
  cc->next(); // eat fix keyword

  if (cc->curr.type != IDENTIFIER)
    return logErrorS("Expected identifier.");

  std::string id = cc->curr.value;
  cc->next(); // eat identifier

  if (cc->curr.type != SEPARATOR)
    return logErrorS("Expected type.");

  cc->next(); // eat separator

  switch (cc->curr.type) {
    case INT_KEYWORD:
      cc->next(); // eat type identifier
      break;
    default:
      return logErrorS("Expected type identifier.");
  }

  if (cc->curr.type != OP_ASSIGN)
    return logErrorS("Expected assignment operator.");

  cc->next(); // eat assignment operator

  std::unique_ptr<Expr> exp = parseExpr(cc);
  if (!exp)
    return nullptr;

  return std::make_unique<AssignStatement>(id, std::move(exp));
}


/**
 * Parse a compound statement.
 * 
 * Creates a special statement node which often represents a function body.
 * 
 * @param cc Token stream.
 * @return   Pointer to compound statement node.
 */
std::unique_ptr<Statement> parse_compound_statement(std::shared_ptr<tstream> cc)
{
  std::vector<std::unique_ptr<Statement>> stmts;

  while (true) {
    if (cc->curr.type == RETURN_KEYWORD) {
      std::unique_ptr<Statement> stmt = parseReturnStatement(cc);
      stmts.push_back(std::move(stmt));
      continue;
    }

    if (cc->curr.type == FIX_KEYWORD) {
      std::unique_ptr<Statement> stmt = parse_declaration(cc);
      stmts.push_back(std::move(stmt));
      cc->next();
      continue;
    }
    
    if (cc->curr.type == END_BLOCK)
      break;
    return logErrorS("Expected end block.");
  }
  
  cc->next(); // eat end block
  return std::make_unique<CompoundStatement>(std::move(stmts));
}


/**
 * Handle a generic definition.
 * 
 * @param cc        Token stream.
 */
void HandleDefinition(std::shared_ptr<tstream> cc) {
  if (std::unique_ptr<FunctionAST> FnAST = parseDefinition(cc)) {
  } else {
    cc->next();
  }
}


/**
 * Handle a top-level expression.
 * 
 * @param cc        Token stream.
 */
void HandleTopLevelExpression(std::shared_ptr<tstream> cc) {
  if (std::unique_ptr<FunctionAST> FnAST = parseTopLevelDefinition(cc)) {
  } else {
    cc->next();
  }
}


/**
 * Parse an abstract syntax tree from a token stream.
 * 
 * @param cc        The token stream to parse through.
 */
void parse(std::shared_ptr<tstream> cc) {
  while (true) {
    switch (cc->curr.type) {
    case TERMINATE_STREAM:
      return;
    case SEMICOLON:
      cc->next();
      break;
    case FUNCTION_KEYWORD:
      HandleDefinition(cc);
      break;
    default:
      HandleTopLevelExpression(cc);
      break;
    }
  }
}
