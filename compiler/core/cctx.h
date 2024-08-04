#ifndef STATIMC_CCTX_H
#define STATIMC_CCTX_H

/// Context for the compiler.
/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "token.h"
#include "symbol.h"
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

  /// Current filename.
  std::string m_filename;

  /// Symbol table.
  std::unique_ptr<SymTable> symb_table = std::make_unique<SymTable>();

  /// Previous token.
  std::unique_ptr<Token> m_prev = std::make_unique<Token>(TokenKind::Eof);

  public:
    cctx(cflags flags, std::vector<cfile> input);

    /// Get the next token in the stream.
    std::unique_ptr<Token> tk_next();

    /// Add a symbol to the symbol table.
    void symb_add(const std::string &name, std::unique_ptr<Symbol> symbol);

    /// Get a symbol from the symbol table.
    [[nodiscard]]
    std::unique_ptr<Symbol> symb_get(const std::string &name);

    /// Delete a symbol from the symbol table.
    bool symb_del(const std::string &name);

    /// Check if a symbol is of a certain type.
    [[nodiscard]]
    bool symb_is(const std::string &name, SymbolType type);

    /// Get the last token in the stream.
    [[nodiscard]]
    inline std::unique_ptr<Token> prev() { return std::move(m_prev); }
  
    /// Get the current filename.
    [[nodiscard]]
    inline std::string filename() const { return m_filename; }

    /// Begin lexing another input file.
    /// @return If the input file could not be found, and true otherwise.
    [[nodiscard]]
    bool lex_file(const std::string filename);
};

#endif  // STATIMC_CCTX_H
