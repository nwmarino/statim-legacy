/// Copyright 2024 Nick Marino (github.com/nwmarino)

#ifndef STATIMC_LEXER_H
#define STATIMC_LEXER_H

#include <memory>
#include <string>

#include "token.h"
#include "tstream.h"

class Lexer {
  std::string src;
  int iter;
  Token tokenize_id();
  Token tokenize_numerical();
  Token tokenize_string();
  Token tokenize_char();

  public:
    /**
     * Construct a statim Lexer object.
     * 
     * @param __srcpath The path to the source file.
     */
    Lexer(std::string __srcpath);

    /**
     * Tokenize the source file.
     * 
     * @return A shared pointer to a tstream object.
     */
    std::shared_ptr<tstream> tokenize();
};

#endif  // STATIMC_LEXER_H
