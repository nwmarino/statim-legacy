/// Copyright 2024 Nick Marino (github.com/nwmarino)

#ifndef STATIMC_CCTX_H
#define STATIMC_CCTX_H

#include <memory>
#include <string>
#include <vector>

#include "token.h"
#include "../lexer/tokenizer.h"

/// Flags for the compiler.
typedef struct {
  bool emit_asm;
} cflags;

/// Input file to the compiler.
typedef struct {
  std::string filename;
  std::string path;
} cfile;

/// Context for the compiler.
class cctx {
  /// Compiler flags.
  cflags flags;

  /// Input files.
  std::vector<cfile> input;

  /// Lexer object.
  std::unique_ptr<Tokenizer> lexer;

  /// Previous token.
  std::unique_ptr<Token> prev = std::make_unique<Token>(TokenKind::Eof);

  public:
    cctx(cflags flags, std::vector<cfile> input);

    /// Get the next token in the stream.
    [[nodiscard]]
    std::unique_ptr<Token> tk_next();

    /// Get the last token in the stream.
    [[nodiscard]]
    inline std::unique_ptr<Token> tk_prev();

    /// Begin lexing another input file.
    /// @return If the input file could be found.
    bool lex_file(const std::string filename);
};

#endif  // STATIMC_CCTX_H
