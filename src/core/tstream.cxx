/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <iostream>
#include <vector>

#include "../include/token.h"
#include "../include/tstream.h"

/**
 * Construct a new token stream.
 * 
 * @param __tokens A token vector to copy.
 */
tstream::tstream(std::vector<Token> __tokens)
{
  Token terminate;
  terminate.type = TERMINATE_STREAM;
  __tokens.push_back(terminate);
  
  this->__tokens = __tokens;
  this->__currit = 0;
  this->curr = __tokens[__currit];
}


/**
 * Iterate to the next token in this buffer.
 */
void tstream::next()
{
  if (curr.type == TERMINATE_STREAM)
    return;

  this->__currit++;
  this->curr = __tokens[__currit];
}


/**
 * Peek at the next token in this buffer.
 * 
 * @return The next token in this buffer.
 */
Token tstream::peek()
{
  if (curr.type == TERMINATE_STREAM)
    return curr;
  return __tokens[__currit + 1];
}


/**
 * Fetch the size of this stream.
 * 
 * @return The size of this stream.
 */
std::size_t tstream::size() {
  return __tokens.size();
}


/**
 * Print the contents of this stream by type.
 */
void tstream::print() {
  std::string result;
  for (int i = 0; i < this->__tokens.size(); i++) {
    switch (this->__tokens[i].type) {
      case SET_PAREN: std::cout << "SET_PAREN\n"; break;
      case END_PAREN: std::cout << "END_PAREN\n"; break;
      case SET_BLOCK: std::cout << "SET_BLOCK\n"; break;
      case END_BLOCK: std::cout << "END_BLOCK\n"; break;
      case SEMICOLON: std::cout << "SEMICOLON\n"; break;
      case SEPARATOR: std::cout << "SEPARATOR\n"; break;
      case COMMA: std::cout << "COMMA\n"; break;
      case ARROW: std::cout << "ARROW\n"; break;
      case OP_ASSIGN: std::cout << "OP_ASSIGN\n"; break;
      case OP_ADD: std::cout << "OP_ADD\n"; break;
      case OP_SUB: std::cout << "OP_SUB\n"; break;
      case OP_MULT: std::cout << "OP_MULT\n"; break;
      case OP_DIV: std::cout << "OP_DIV\n"; break;
      case OP_POW: std::cout << "OP_POW\n"; break;
      case BOOL_KEYWORD: std::cout << "BOOL_KEYWORD\n"; break;
      case INT_KEYWORD: std::cout << "INT_KEYWORD\n"; break;
      case STRING_KEYWORD: std::cout << "STR_KEYWORD\n"; break;
      case FLOAT_KEYWORD: std::cout << "FLOAT_KEYWORD\n"; break;
      case CHAR_KEYWORD: std::cout << "CHAR_KEYWORD\n"; break;
      case LET_KEYWORD: std::cout << "LET_KEYWORD\n"; break;
      case FIX_KEYWORD: std::cout << "FIX_KEYWORD\n"; break;
      case FUNCTION_KEYWORD: std::cout << "FUNCTION_KEYWORD\n"; break;
      case RETURN_KEYWORD: std::cout << "RETURN_KEYWORD\n"; break;
      case IF_KEYWORD: std::cout << "IF_KEYWORD\n"; break;
      case ELSE_KEYWORD: std::cout << "ELSE_KEYWORD\n"; break;
      case COMMENT: std::cout << "COMMENT\n"; break;
      case IDENTIFIER: std::cout << "IDENTIFIER " << this->__tokens[i].value << '\n'; break;
      case C_BOOL: std::cout << "C_BOOL " << this->__tokens[i].value << '\n'; break;
      case C_INT: std::cout << "C_INT " << this->__tokens[i].value << '\n'; break;
      case C_FP: std::cout << "C_FP " << this->__tokens[i].value << '\n'; break;
      case C_STR: std::cout << "C_STR " << this->__tokens[i].value << '\n'; break;
      case C_CHAR: std::cout << "C_CHAR " << this->__tokens[i].value << '\n'; break;
      case TERMINATE_STREAM: break;
    }
  }
}
