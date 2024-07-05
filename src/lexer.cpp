// Copyright (c) 2024 Nick Marino (github.com/nwmarino)

#include <fstream>
#include <iostream>
#include <string>
#include <stdexcept>
#include <exception>
#include <vector>
#include <map>

#include "lexer.h"
#include "token.h"
#include "tstream.h"

using std::ifstream;
using std::string;
using std::throw_with_nested;
using std::runtime_error;
using std::invalid_argument;
using std::map;

Lexer::Lexer(string __srcpath) {
  ifstream file(__srcpath);

  if (!file.is_open()) {
    throw_with_nested(runtime_error("Unable to open file " + __srcpath));
  }

  string line;
  while (getline(file, line)) {
    src.append(line);
  }
  this->iter = 0;
}

tstream Lexer::tokenize() {
  vector<Token> tokens;

  while (iter < src.size()) {
    Token token;

    switch (src[iter]) {
      case ' ':
        iter++;
        continue;
      case '{':
        token.type = SetBrace;
        break;
      case '}':
        token.type = EndBrace;
        break;
      case '(':
        token.type = SetParen;
        break;
      case ')':
        token.type = EndParen;
        break;
      case ';':
        token.type = Semicolon;
        break;
      case ',':
        token.type = Comma;
        break;
      case ':':
        token.type = Separator;
        break;
      case '\'':
        tokens.push_back(tokenize_char());
        continue;
      case '"':
        tokens.push_back(tokenize_string());
        continue;
      case '=':
        token.type = AssignOperator;
        break;
      case '+':
        token.type = AddOperator;
        break;
      case '-':
        if ((iter + 1) < src.size() && src[iter + 1] == '>') {
          token.type = Arrow;
          iter++;
        } else {
          token.type = SubOperator;
        }
        break;
      case '*':
        if ((iter + 1) < src.size() && src[iter + 1] == '*') {
          token.type = PowerOperator;
          iter++;
        } else {
          token.type = MultOperator;
        }
        break;
      case '/':
        if ((iter + 1) < src.size() && src[iter + 1] == '/') {
          token.type = Comment;
          iter++;
        } else {
          token.type = DivOperator;
        }
        break;
      default:
        if (isalpha(src[iter])) {
          tokens.push_back(tokenize_id());
          continue;
        } else if (isdigit(src[iter])) {
          tokens.push_back(tokenize_numerical());
          continue;
        } else {
          throw_with_nested(invalid_argument("Unresolved character " + src[iter]));
        }
        break;
    }
    tokens.push_back(token);
    iter++;
  }

  return tstream(tokens);
}

Token Lexer::tokenize_id() {
  Token token;
  map<string, TokenType> keywords;
  keywords["fn"] = FunctionKeyword;
  keywords["return"] = ReturnKeyword;
  keywords["bool"] = BoolKeyword;
  keywords["int"] = IntKeyword;
  keywords["float"] = FloatKeyword;
  keywords["string"] = StringKeyword;
  keywords["char"] = CharKeyword;
  keywords["let"] = LetKeyword;
  keywords["fix"] = FixKeyword;

  string id;
  while (iter < src.size() && isalpha(src[iter])) {
    id.push_back(src[iter]);
    iter++;
  }

  if (keywords.find(id) != keywords.end()) {
    token.type = keywords[id];
  } else {
    token.value = id;
    if (id == "true" || id == "false")
      token.type = Boolean;
    else
      token.type = Identifier;
  }

  return token;
}

Token Lexer::tokenize_numerical() {
  Token token;
  token.type = Integer;

  string numerical;
  while (iter < src.size() && src[iter] != ' ' && src[iter] != ';') {
    if (!isdigit(src[iter]) && src[iter] != '.') {
      throw_with_nested(invalid_argument("Unresolved numerical " + numerical));
    }

    numerical.push_back(src[iter]);
    
    if (src[iter] == '.') {
      token.type = Float;
    }

    iter++;
  }

  token.value = numerical;
  return token;
}

Token Lexer::tokenize_string() {
  Token token;
  token.type = String;
  string contents;
  iter++;  // skip first delimiter
  while (iter < src.size() && src[iter] != '"') {
    contents.push_back(src[iter]);
    iter++;
  }

  if (src[iter] != '"') {
    throw_with_nested(invalid_argument("Unresolved string " + contents));
  }

  token.value = contents;
  iter++;
  return token;
}

Token Lexer::tokenize_char() {
  Token token;
  token.type = Char;

  iter++;
  token.value = src[iter];

  if (src[iter + 1] != '\'') {
    throw_with_nested(invalid_argument("Unresolved char " + token.value));
  }

  iter += 2;
  return token;
}
