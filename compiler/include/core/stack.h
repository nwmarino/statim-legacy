#ifndef STATIMC_STACK_H
#define STATIMC_STACK_H

/// Symbol table stack data structure.
/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <memory>
#include <vector>

#include "symbol.h"

class SymTableStack {
  std::vector<std::unique_ptr<SymTable>> list;

  public:
    SymTableStack() : list(std::vector<std::unique_ptr<SymTable>>()) {};

    /// Push a new value onto the stack.
    inline void push(std::unique_ptr<SymTable> table) { list.push_back(std::move(table)); }

    /// Pop a value off the stack.
    inline std::unique_ptr<SymTable> pop() {
      std::unique_ptr<SymTable> table = std::move(list.back());
      list.pop_back();
      return std::move(table);
    }

    /// Peek at the top of the stack.
    [[nodiscard]]
    inline std::unique_ptr<SymTable> peek() { return std::move(list.back()); }

    /// Add a symbol to the table in the current scope.
    inline void add(const std::string &name, const Symbol &symbol) {
      list.back()->put(name, symbol);
    }

    /// Check if the stack is empty.
    [[nodiscard]]
    inline bool is_empty() const { return list.empty(); }

    /// Check if a symbol exists in the stack, and returns the symbol.
    [[nodiscard]]
    inline std::unique_ptr<Symbol> find(const std::string &name) {
      for (std::unique_ptr<SymTable> &table : list) {
        if (table->exists(name)) {
          return std::make_unique<Symbol>(table->get(name));
        }
      }
      return nullptr;
    }
};

#endif  // STATIMC_STACK_H
