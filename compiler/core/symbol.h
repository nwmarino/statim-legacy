#ifndef STATIMC_SYMBOL_H
#define STATIMC_SYMBOL_H

/// Symbols and the symbol table.
/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <map>
#include <string>
#include <utility>

#include "token.h"

/// A symbol type.
typedef enum SymbolType {
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
  Ty
} SymbolType;

/// A recognized smybol in a program.
struct Symbol {
  /// The symbol type.
  SymbolType type;

  /// Associated metadata.
  struct Metadata meta;

  /// Possible keyword type.
  KeywordType keyword;

  // Compiler-defined constructor.
  inline Symbol(SymbolType type) : type(type) {};

  // Basic user-defined constructor.
  inline Symbol(SymbolType type, const Metadata &meta) : type(type), meta(meta) {};

  // Keyword constructor.
  inline Symbol(KeywordType keyword) : type(SymbolType::Keyword), keyword(keyword) {};
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
