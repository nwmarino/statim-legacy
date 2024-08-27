#ifndef STATIMC_SYMBOL_H
#define STATIMC_SYMBOL_H

/// Symbols and the symbol table.
/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <map>
#include <string>
#include <utility>

#include "../token/Token.h"

/// Recognized kinds of symbols.
typedef enum SymbolKind {
  /// Mutable variables
  Variable,

  /// Immutable constants
  Constant,

  /// Function definitions
  Function,

  /// Keywords
  Keyword,

  /// Abstracts
  Interface,

  /// Type definitions (structs, enums, unions)
  TypeDef,

  /// Package
  Package
} SymbolKind;

// An enum of recognized keywords.
typedef enum KeywordKind {
  /// abstract
  Abstract,

  /// bind
  Bind,

  /// break
  Break,

  /// continue
  Continue,

  /// else
  Else,

  /// end
  End,

  /// enum
  Enum,

  /// extern
  Extern,

  /// false
  False,

  /// fix
  Fix,

  /// fn
  Fn,

  /// for
  For,

  /// if
  If,

  /// impl
  Impl,

  /// let
  Let,

  /// match
  Match,

  /// pkg
  Pkg,

  /// protect
  Protect,

  /// return
  Return,

  /// struct
  Struct,

  /// true
  True,

  /// union
  Union,

  /// until
  Until,

  /// use
  Use,

  /// unknown keyword
  Unknown
} KeywordKind;

/// A type definition kind.
typedef enum TypeDefKind {
  /// A struct type.
  TY_STRUCT,

  /// An enum type.
  TY_ENUM,

  /// A union type.
  TY_UNION
} TypeDefKind;

/// A recognized symbol in a program.
struct Symbol {
  /// The symbol type.
  SymbolKind kind;

  /// Associated metadata.
  struct Metadata meta;

  /// Possible keyword type.
  KeywordKind keyword;

  /// Possible type kind.
  TypeDefKind type_kind;

  // Compiler-defined constructor.
  inline Symbol(SymbolKind  kind) : kind(kind) {};

  // Basic user-defined constructor.
  inline Symbol(SymbolKind kind, const Metadata &meta) : kind(kind), meta(meta) {};

  // Type symbol constructor.
  inline Symbol(TypeDefKind kind, const Metadata &meta) : kind(SymbolKind::TypeDef), meta(meta), type_kind(kind) {};

  // Keyword constructor.
  inline Symbol(KeywordKind keyword) : kind(SymbolKind::Keyword), keyword(keyword) {};
};

/// A table of symbols.
class SymTable {
  std::map<const std::string, struct Symbol> symbs = {};
  std::size_t m_size;

  public:
    inline SymTable() : m_size(0) {};

    /// Put a symbol into the table.
    inline void put(const std::string key, const struct Symbol &s) {
      symbs.insert({key, s});
      m_size++;
    }

    /// Remove a symbol from the table.
    inline struct Symbol remove(const std::string key) {
      struct Symbol symb = symbs.at(key);
      symbs.erase(key);
      m_size--;
      return std::move(symb);
    }

    /// Get a symbol from the table.
    [[nodiscard]]
    inline struct Symbol get(const std::string key) {
      return symbs.at(key);
    }

    /// Delete a symbol from the table.
    inline bool del(const std::string key) {
      return symbs.erase(key) == 1;
    }

    /// Check if a symbol exists in the table.
    [[nodiscard]]
    inline bool exists(const std::string key) {
      return symbs.find(key) != symbs.end();
    }

    /// Get the size of this table.
    [[nodiscard]]
    inline std::size_t size() const { return m_size; }
};

#endif  // STATIMC_SYMBOL_H
