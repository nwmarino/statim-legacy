#ifndef STATIMC_SYMBOL_H
#define STATIMC_SYMBOL_H

/// Symbols and the symbol table.
/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <map>
#include <memory>
#include <string>
#include <utility>

/// A symbol type.
typedef enum SymbolType {
  /// Mutable variables.
  Variable,

  /// Immutable constants.
  Constant,

  /// Function definitions.
  Function
} SymbolType;

/// A recognized smybol in a program.
typedef struct Symbol {
  SymbolType type;
} Symbol;

/// A table of symbols.
class SymTable {
  std::map<const std::string, std::unique_ptr<Symbol>> symbs = {};
  std::size_t m_size;

  public:
    inline SymTable() : m_size(0) {};

    /// Put a symbol into the table.
    inline void put(const std::string key, std::unique_ptr<Symbol> s) {
      symbs.at(key) = std::move(s);
      m_size++;
    }

    /// Remove a symbol from the table.
    inline std::unique_ptr<Symbol> remove(const std::string key) {
      std::unique_ptr symb = std::move(symbs.at(key));
      symbs.erase(key);
      m_size--;
      return std::move(symb);
    }

    /// Get a symbol from the table.
    [[nodiscard]]
    inline Symbol *get(const std::string key) {
      return symbs.at(key).get();
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
