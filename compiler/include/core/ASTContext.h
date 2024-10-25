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
  unsigned int debug : 1;
  unsigned int emit_llvm_ir : 1;
  unsigned int emit_asm : 1;
  unsigned int dump_ast : 1;
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
  bool _add_next_to_scope;
  std::string _top_impl;
  bool _past_base;
  std::map<const std::string, Type *> type_table;

public:
  ASTContext(struct CFlags flags, std::vector<struct CFile> input);
  /// Gets the next token in the stream.
  struct Token next(void);
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
  void next_file(void);
  /// Returns true if the next named declaration should be added to the parent scope.
  [[nodiscard]]
  inline bool add_next_to_scope(void) const { return _add_next_to_scope; }
  /// Declare that the next should be added to the parent scope, or not.
  void set_add_next_to_scope(bool add);
  /// Returns the name of the current top-level implementation, and an empty string if there is none.
  [[nodiscard]]
  inline std::string top_impl(void) const { return _top_impl; }
  /// Sets the name of the current top-level implementation.
  void set_top_impl(const std::string &name);
  /// Returns true if the parser has past a base identifier. Used to parse chained member expressions.
  [[nodiscard]]
  inline bool past_base(void) const { return _past_base; }
  /// Sets the past base flag.
  void set_past_base(bool past);
  /// Resolves a type by name. Returns a `TypeRef` object if the type is not found.
  [[nodiscard]]
  Type* resolve_type(const std::string &name);
  /// Declares a type in the type table. Used for source defined types. Panics if the type already exists.
  /// @returns A pointer to the new type.
  Type* declare_type(const std::string &name, Type *T);
};

#endif  // ASTCONTEXT_STATIMC_H
