/// Copyright 2024 Nick Marino (github.com/nwmarino)

#ifndef STATIMC_TOKENIZER_H
#define STATIMC_TOKENIZER_H

#include <string>

#include "../core/token.h"

/// Iterative token parser.
class Tokenizer {
  const std::string src;
  const std::string filename;
  const std::size_t len;
  char prev;
  std::size_t iter;
  std::size_t line;
  
  /// Peek at the next character in the stream. Does not consume it.
  [[nodiscard]]
  const char peek() const;

  /// Peek at the second character in the stream. Does not consume it.
  [[nodiscard]]
  const char peek_two() const;

  /// Peek at the third character in the stream. Does not consume it.
  [[nodiscard]]
  const char peek_three() const;

  [[nodiscard]]
  inline const bool is_newl();

  [[nodiscard]]
  inline const bool is_eof();

  public:
    /// Default constructor.
    Tokenizer(const std::string src, const std::string filename, const std::size_t len);

    /// Get the next token in the stream.
    const struct Token advance_token();

    [[nodiscard]]
    inline const std::string to_str();
};

#endif  // STATIMC_TOKENIZER_H
