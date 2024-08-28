#ifndef CCONTEXT_STATIMC_H
#define CCONTEXT_STATIMC_H

/// Context for the compiler.
/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include "Utils.h"
#include "../token/Tokenizer.h"
#include <iostream>

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
    struct Token _last_two;
    std::string _file;
    std::unique_ptr<Scope> _parent_scope;

  public:
    /// Default constructor.
    CContext(struct CFlags flags, std::vector<struct CFile> input) : flags(flags), input(input), _last(Token(Eof)), _last_two(Token(Eof)) {};
  
    /// Get the next token in the stream.
    struct Token next(void) {
      _last_two = _last;
      return _last = lexer->advance_token(); 
    }

    /// Get the last token in the stream.
    [[nodiscard]]
    inline struct Token last(void) const { return _last; }

    /// Get the second to last token in the stream.
    [[nodiscard]]
    inline struct Token last_two(void) const { return _last_two; }

    /// Get the name of the currently open file.
    [[nodiscard]]
    inline std::string file(void) const { return _file; }

    /// Move to the next input file.
    inline void next_file(void) { 
      if (input.size() != 0) {
        std::string f_src = read_to_str(input.at(input.size() - 1).path);
        _file = input.at(input.size() - 1).filename;
        lexer = std::make_unique<Tokenizer>(f_src, input.at(input.size() - 1).filename, f_src.size());
        next();
        input.pop_back();
      }
    }

    /// Assign a new parent scope.
    inline void set_parent_scope(std::unique_ptr<Scope> &scope) {
      if (scope) { 
        _parent_scope = std::move(scope);
        return;
      }
      _parent_scope = nullptr;
    }

    /// Get the current parent scope.
    [[nodiscard]]
    inline std::unique_ptr<Scope> &parent_scope(void) { return _parent_scope; }
};

#endif  // CCONTEXT_STATIMC_H
