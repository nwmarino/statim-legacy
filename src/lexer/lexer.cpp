/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <exception>
#include <fstream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "../include/lexer.h"
#include "../include/token.h"
#include "../include/tstream.h"

/**
 * Construct a statim Lexer object.
 * 
 * @param __srcpath The path to the source file.
 */
Lexer::Lexer(std::string __srcpath) {
  std::ifstream file(__srcpath);

  if (!file.is_open()) {
    std::throw_with_nested(std::invalid_argument("Unable to open file " + __srcpath));
  }

  std::string line;
  while (getline(file, line)) {
    src.append(line);
  }
  this->iter = 0;
}


/**
 * Tokenize the source file.
 * 
 * @return A shared pointer to a tstream object.
 */
std::shared_ptr<tstream> Lexer::tokenize() {
  std::vector<Token> tokens;

  while (iter < src.size()) {
    Token token;

    switch (src[iter]) {
      case ' ':
        iter++;
        continue;
      case '{':
        token.type = SET_BLOCK;
        break;
      case '}':
        token.type = END_BLOCK;
        break;
      case '(':
        token.type = SET_PAREN;
        break;
      case ')':
        token.type = END_PAREN;
        break;
      case ';':
        token.type = SEMICOLON;
        break;
      case ',':
        token.type = COMMA;
        break;
      case ':':
        token.type = SEPARATOR;
        break;
      case '\'':
        tokens.push_back(tokenize_char());
        continue;
      case '"':
        tokens.push_back(tokenize_string());
        continue;
      case '=':
        token.type = OP_ASSIGN;
        break;
      case '+':
        token.type = OP_ADD;
        break;
      case '-':
        if ((iter + 1) < src.size() && src[iter + 1] == '>') {
          token.type = ARROW;
          iter++;
        } else {
          token.type = OP_SUB;
        }
        break;
      case '*':
        if ((iter + 1) < src.size() && src[iter + 1] == '*') {
          token.type = OP_POW;
          iter++;
        } else {
          token.type = OP_MULT;
        }
        break;
      case '/':
        if ((iter + 1) < src.size() && src[iter + 1] == '/') {
          token.type = COMMENT;
          iter++;
        } else {
          token.type = OP_DIV;
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
          std::throw_with_nested(std::invalid_argument(&"Unresolved character " [src[iter]]));
        }
        break;
    }
    tokens.push_back(token);
    iter++;
  }

  return std::make_shared<tstream>(tokens);
}


/**
 * Tokenize an identifier.
 * 
 * @return The tokenized identifier.
 */
Token Lexer::tokenize_id() {
  Token token;
  std::map<std::string, TokenType> keywords;
  keywords["bool"] = BOOL_KEYWORD;
  keywords["char"] = CHAR_KEYWORD;
  keywords["else"] = ELSE_KEYWORD;
  keywords["fix"] = FIX_KEYWORD;
  keywords["float"] = FLOAT_KEYWORD;
  keywords["fn"] = FUNCTION_KEYWORD;
  keywords["if"] = IF_KEYWORD;
  keywords["int"] = INT_KEYWORD;
  keywords["let"] = LET_KEYWORD;
  keywords["string"] = STRING_KEYWORD;
  keywords["return"] = RETURN_KEYWORD;
  
  std::string id;
  while (iter < src.size() && isalpha(src[iter])) {
    id.push_back(src[iter]);
    iter++;
  }

  if (keywords.find(id) != keywords.end()) {
    token.type = keywords[id];
  } else {
    token.value = id;
    if (id == "true" || id == "false")
      token.type = C_BOOL;
    else
      token.type = IDENTIFIER;
  }

  return token;
}


/**
 * Tokenize a numerical value.
 * 
 * @return The tokenized numerical value.
 */
Token Lexer::tokenize_numerical() {
  Token token;
  token.type = C_INT;

  std::string numerical;
  while (iter < src.size() && src[iter] != ' ' && src[iter] != ';') {
    if (!isdigit(src[iter]) && src[iter] != '.') {
      std::throw_with_nested(std::invalid_argument("Unresolved numerical " + numerical));
    }

    numerical.push_back(src[iter]);
    
    if (src[iter] == '.') {
      token.type = C_FP;
    }

    iter++;
  }

  token.value = numerical;
  return token;
}


/**
 * Tokenize a string.
 * 
 * @return The tokenized string.
 */
Token Lexer::tokenize_string() {
  Token token;
  token.type = C_STR;
  std::string contents;
  iter++;  // skip first delimiter
  while (iter < src.size() && src[iter] != '"') {
    contents.push_back(src[iter]);
    iter++;
  }

  if (src[iter] != '"') {
    std::throw_with_nested(std::invalid_argument("Unresolved string " + contents));
  }

  token.value = contents;
  iter++;
  return token;
}


/**
 * Tokenize a char.
 * 
 * @return The tokenized char.
 */
Token Lexer::tokenize_char() {
  Token token;
  token.type = C_CHAR;

  iter++;
  token.value = src[iter];

  if (src[iter + 1] != '\'') {
    std::throw_with_nested(std::invalid_argument("Unresolved char " + token.value));
  }

  iter += 2;
  return token;
}
