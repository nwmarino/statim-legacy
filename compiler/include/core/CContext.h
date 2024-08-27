#ifndef CCONTEXT_STATIMC_H
#define CCONTEXT_STATIMC_H

/// Context for the compiler.
/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include "../token/Tokenizer.h"
#include "Utils.h"

/// A list of flags during the compilation process.
struct CFlags
{
  bool debug;
  bool emit_asm;
};


/// An input file to the compiler.
struct CFile
{
  std::string filename;
  std::string path;
};


/// Context wrapper for the compiler.
struct CContext
{
  private:
    struct CFlags flags;
    std::vector<struct CFile> input;
    std::unique_ptr<Tokenizer> lexer;
    struct Token _last;

  public:
    /// Default constructor.
    CContext(struct CFlags flags, std::vector<struct CFile> input) : flags(flags), input(input), _last(Token(Eof)) {};
  
    /// Get the next token in the stream.
    struct Token next(void) { return _last = lexer->advance_token(); }

    /// Get the last token in the stream.
    [[nodiscard]]
    inline struct Token last(void) const { return _last; }

    /// Move to the next input file.
    inline void next_file(void) { 
      if (input.size() != 0) {
        std::string f_src = read_to_str(input.at(input.size() - 1).path);
        lexer = std::make_unique<Tokenizer>(f_src, input.at(input.size() - 1).filename, f_src.size());
        next();
        input.pop_back();
      }
    }
};

#endif  // CCONTEXT_STATIMC_H
