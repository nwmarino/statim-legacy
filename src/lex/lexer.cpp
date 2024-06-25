// Copyright (c) 2024 Nick Marino (github.com/nwmarino)

#include <fstream>
#include <iostream>
#include <string>
#include <exception>
#include <vector>

#include "lexer.h"
#include "token.h"

using std::ifstream;
using std::string;

Lexer::Lexer(string __srcpath) {
  ifstream file(__srcpath);

  if (!file.is_open()) {
    std::throw_with_nested(std::runtime_error("Unable to open file: " + __srcpath));
  }

  string line;
  while (getline(file, line)) {
    src.append(line);
  }
}

vector<Token> Lexer::tokenize()
{
  return vector<Token>();
}
