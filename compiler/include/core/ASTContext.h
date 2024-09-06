#ifndef ASTCONTEXT_STATIMC_H
#define ASTCONTEXT_STATIMC_H

/// AST-related context for the parsing processes.
/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <map>
#include <memory>
#include <vector>

#include "Type.h"
#include "../token/Tokenizer.h"

/// CFlags - A list of flags during the compilation process.
///
/// The CFlags struct contains a list of flags that can be set during the compilation process.
/// These flags are used to control the behavior of the compiler and the output of the program.
struct CFlags {
  bool debug;
  bool emit_llvm_ir;
  bool emit_asm;
};


/// CFile - An input file wrapper for the compiler.
///
/// The CFile struct contains the filename and path of an input file that is being compiled.
struct CFile {
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
  std::map<const std::string, Type *> type_table;

public:
  ASTContext(struct CFlags flags, std::vector<struct CFile> input);
  /// Gets the next token in the stream.
  struct Token next(void);
  /// Gets the last token in the stream.
  [[nodiscard]]
  inline struct Token last(void) const;
  /// Gets the second to last token in the stream.
  [[nodiscard]]
  inline struct Token last_two(void) const;
  /// Gets the name of the currently open file.
  [[nodiscard]]
  inline std::string file(void) const;
  /// Moves to the next input file and consumes the previous.
  void next_file(void);
  /// Resolves a type by name. Returns a `TypeRef` object if the type is not found.
  [[nodiscard]]
  Type* resolve_type(const std::string &name);
};

#endif  // ASTCONTEXT_STATIMC_H
