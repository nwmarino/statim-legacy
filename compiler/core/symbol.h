#ifndef STATIMC_SYMBOL_H
#define STATIMC_SYMBOL_H

/// Symbols and the symbol table.
/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <map>
#include <memory>
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

  // Keywords
  Keyword,

  // Type definitions
  Ty
} SymbolType;

/// A recognized smybol in a program.
struct Symbol {
  /// The symbol type.
  SymbolType type;

  /// Associated metadata.
  std::unique_ptr<Metadata> meta;

  /// Possible keyword type.
  KeywordType keyword;

  // Compiler-defined constructor.
  inline Symbol(SymbolType type) : type(type), meta(nullptr) {};

  // Basic user-defined constructor.
  inline Symbol(SymbolType type, std::unique_ptr<Metadata> meta) : type(type), meta(std::move(meta)) {};

  // Keyword constructor.
  inline Symbol(KeywordType keyword) : type(SymbolType::Keyword), keyword(keyword) {};
};

/// A table of symbols.
class SymTable {
  std::map<const std::string, std::unique_ptr<Symbol>> symbs = {};
  std::size_t m_size;

  public:
    inline SymTable() : m_size(0) {};

    /// Put a symbol into the table.
    inline void put(const std::string key, std::unique_ptr<Symbol> s) {
      symbs.insert({key, std::move(s)});
      m_size++;
    }

    /// Remove a symbol from the table.
    inline std::unique_ptr<Symbol> remove(const std::string key) {
      // check if the symbol exists
      if (exists(key)) {
        std::unique_ptr symb = std::move(symbs.at(key));
        symbs.erase(key);
        m_size--;
        return std::move(symb);
      }
      return nullptr;
    }

    /// Get a symbol from the table.
    [[nodiscard]]
    inline std::unique_ptr<Symbol> get(const std::string key) {
      if (exists(key)) {
        return std::move(symbs.at(key));
      }
      
      return nullptr;
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
