#ifndef ASTCONTEXT_STATIMC_H
#define ASTCONTEXT_STATIMC_H

/// AST-related context for the parsing processes.
/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <memory>
#include <vector>

#include "Utils.h"
#include "../token/Tokenizer.h"

/// CFlags - A list of flags during the compilation process.
///
/// The CFlags struct contains a list of flags that can be set during the compilation process.
/// These flags are used to control the behavior of the compiler and the output of the program.
struct CFlags
{
  bool debug;
  bool emit_asm;
};


/// CFile - A file wrapper for the compiler.
///
/// The CFile struct contains the filename and path of an input file that is being compiled.
struct CFile
{
  std::string filename;
  std::string path;
};


/// ASTContext - Context for the compiler during the parsing process.
///
/// The ASTContext struct contains the context for the AST and the current state of the lexer.
/// This context is used to keep track of the current state of the lexer and the current token.
struct ASTContext
{
  private:
    struct CFlags flags;
    std::vector<struct CFile> input;
    std::unique_ptr<Tokenizer> lexer;
    struct Token _last;
    struct Token _last_two;
    std::string _file;

  public:
    ASTContext(struct CFlags flags, std::vector<struct CFile> input) : flags(flags), input(input), _last(Token(Eof)), _last_two(Token(Eof)) {};
  
    /// Gets the next token in the stream.
    struct Token next(void) {
      _last_two = _last;
      return _last = lexer->advance_token(); 
    }

    /// Gets the last token in the stream.
    [[nodiscard]]
    inline struct Token last(void) const { return _last; }

    /// Gets the second to last token in the stream.
    [[nodiscard]]
    inline struct Token last_two(void) const { return _last_two; }

    /// Gets the name of the currently open file.
    [[nodiscard]]
    inline std::string file(void) const { return _file; }

    /// Moves to the next input file and consumes the previous.
    inline void next_file(void) { 
      if (input.size() != 0) {
        std::string f_src = read_to_str(input.at(input.size() - 1).path);
        _file = input.at(input.size() - 1).filename;
        lexer = std::make_unique<Tokenizer>(f_src, input.at(input.size() - 1).filename, f_src.size());
        next();
        input.pop_back();
      }
    }
};

#endif  // ASTCONTEXT_STATIMC_H
