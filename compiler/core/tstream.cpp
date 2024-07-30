/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <memory>
#include <utility>
#include <iostream>
#include <vector>

#include "token.h"
#include "tstream.h"

/**
 * Construct a new token stream.
 * 
 * @param __tokens A token vector to copy.
 */
tstream::tstream(std::vector<Token> __tokens)
{
  Token eof;
  eof.kind = Eof;
  __tokens.push_back(eof);
  
  this->__tokens = __tokens;
  this->__currit = 0;
  this->curr = __tokens[__currit];
}


/**
 * Iterate to the next token in this buffer.
 */
void tstream::next()
{
  if (curr.kind == Eof)
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
  if (curr.kind == Eof)
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
  for (int i = 0; i < this->__tokens.size(); i++) {
    switch (this->__tokens[i].kind) {
      case Whitespace: break;
      case OpenParen: std::cout << "OpenParen\n"; break;
      case CloseParen: std::cout << "CloseParen\n"; break;
      case OpenBrace: std::cout << "OpenBrace\n"; break;
      case CloseBrace: std::cout << "CloseBrace\n"; break;
      case OpenBracket: std::cout << "OpenBracket\n"; break;
      case CloseBracket: std::cout << "CloseBracket\n"; break;
      case Semicolon: std::cout << "Semicolon\n"; break;
      case Colon: std::cout << "Colon\n"; break;
      case Period: std::cout << "Period\n"; break;
      case Comma: std::cout << "Comma\n"; break;
      case At: std::cout << "At\n"; break;
      case Hash: std::cout << "Hash\n"; break;
      case Arrow: std::cout << "Arrow\n"; break;
      case Eq: std::cout << "Eq\n"; break;
      case EqEq: std::cout << "EqEq\n"; break;
      case Bang: std::cout << "Bang\n"; break;
      case BangEq: std::cout << "BangEq\n"; break;
      case LessThan: std::cout << "LessThan\n"; break;
      case LessThanEq: std::cout << "LessThanEq\n"; break;
      case GreaterThan: std::cout << "GreaterThan\n"; break;
      case GreaterThanEq: std::cout << "GreaterThanEq\n"; break;
      case LeftShift: std::cout << "LeftShift\n"; break;
      case RightShift: std::cout << "RightShift\n"; break;
      case And: std::cout << "And\n"; break;
      case Or: std::cout << "Or\n"; break;
      case Xor: std::cout << "Xor\n"; break;
      case Add: std::cout << "Add\n"; break;
      case Sub: std::cout << "Sub\n"; break;
      case Star: std::cout << "Star\n"; break;
      case Slash: std::cout << "Slash\n"; break;
      case Power: std::cout << "Power\n"; break;
      case Increment: std::cout << "Increment\n"; break;
      case Decrement: std::cout << "Decrement\n"; break;
      case LineComment: std::cout << "LineComment\n"; break;
      case BlockComment: std::cout << "BlockComment\n"; break;
      case Identifier: std::cout << "Identifier " << this->__tokens[i].value << '\n'; break;
      case Literal: std::cout << "C_BOOL " << this->__tokens[i].value << '\n'; break;
      case Eof: break;
    }
  }
}
