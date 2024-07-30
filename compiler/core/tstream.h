/// Copyright 2024 Nick Marino (github.com/nwmarino)

#ifndef STATIMC_TSTREAM_H
#define STATIMC_TSTREAM_H

#include <vector>

#include "token.h"

/**
 * An iterative stream consisting of Tokens.
 */
class tstream {
  std::vector<Token> __tokens;
  std::size_t __currit;

  public:
    /// The current token in this buffer.
    Token curr;

    /**
     * Construct a new token stream.
     * 
     * @param __tokens A token vector to copy.
     */
    tstream(std::vector<Token> __tokens);
    
    /// Iterate to the next token in this buffer.
    void next();

    /**
     * Peek at the next token in this buffer.
     * 
     * @return The next token in this buffer.
     */
    Token peek();

    /**
     * Fetch the size of this stream.
     * 
     * @return The size of this stream.
     */
    std::size_t size();

    /// Print the contents of this stream by type.
    void print();
};

#endif  // STATIMC_TSTREAM_H
