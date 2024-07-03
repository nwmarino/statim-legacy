// Copyright (c) 2024 Nick Marino (github.com/nwmarino)

#include <vector>

#include "token.h"
#include "tstream.h"

using std::vector;
using std::size_t;

tstream::tstream(vector<Token> __tokens) {
  Token terminate;
  terminate.type = Terminate;
  __tokens.push_back(terminate);
  
  this->__tokens = __tokens;
  this->__currit = 0;
}

Token tstream::next() {
  if (__tokens[__currit].type == Terminate) {
    return __tokens[__currit];
  }
  __currit++;
  return __tokens[__currit - 1];
}

Token tstream::curr() {
  return __tokens[__currit];
}

void tstream::skip(size_t __count) {
  int i = 0;
  while (__tokens[__currit].type != Terminate && i < __count) {
    i++;
    __currit++;
  }
}

size_t tstream::size() {
  return __tokens.size();
}

string tstream::debugstr() {
  string result;
  for (int i = 0; i < this->__tokens.size(); i++) {
    switch (this->__tokens[i].type) {
      case SetBrace:
        result.append("SetBrace\n");
        break;
      case EndBrace:
        result.append("EndBrace\n");
        break;
      case SetParen:
        result.append("SetParen\n");
        break;
      case EndParen:
        result.append("EndParen\n");
        break;
      case Semicolon:
        result.append("Semicolon\n");
        break;
      case Separator:
        result.append("Separator\n");
        break;
      case Identifier:
        result.append("Identifier " + this->__tokens[i].value + "\n");
        break;
      case Arrow:
        result.append("Arrow\n");
        break;
      case AssignOperator:
        result.append("AssignOperator\n");
        break;
      case AddOperator:
        result.append("AddOperator\n");
        break;
      case SubOperator:
        result.append("SubOperator\n");
        break;
      case MultOperator:
        result.append("MultOperator\n");
        break;
      case DivOperator:
        result.append("DivOperator\n");
        break;
      case PowerOperator:
        result.append("PowerOperator\n");
        break;
      case Comment:
        result.append("Comment\n");
        break;
      case Integer:
        result.append("Integer " + this->__tokens[i].value + "\n");
        break;
      case Float:
        result.append("Float " + this->__tokens[i].value + "\n");
        break;
      case Boolean:
        result.append("Boolean " + this->__tokens[i].value + "\n");
        break;
      case String:
        result.append("String " + this->__tokens[i].value + "\n");
        break;
      case Char:
        result.append("Char " + this->__tokens[i].value + "\n");
        break;
      case FunctionKeyword:
        result.append("FunctionKeyword\n");
        break;
      case ReturnKeyword:
        result.append("ReturnKeyword\n");
        break;
      case BoolKeyword:
        result.append("BoolKeyword\n");
        break;
      case IntKeyword:
        result.append("IntKeyword\n");
        break;
      case FloatKeyword:
        result.append("FloatKeyword\n");
        break;
      case StringKeyword:
        result.append("StringKeyword\n");
        break;
      case CharKeyword:
        result.append("CharKeyword\n");
        break;
      case LetKeyword:
        result.append("LetKeyword\n");
        break;
      case FixKeyword:
        result.append("FixKeyword\n");
        break;
    }
  }
  return result;
}
