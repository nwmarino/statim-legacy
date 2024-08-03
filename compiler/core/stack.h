#ifndef STATIMC_STACK_H
#define STATIMC_STACK_H

/// Stack data structure.
/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <memory>
#include <vector>

template <typename T> class Stack {
  std::vector<std::unique_ptr<T>> stack;

  public:
    Stack() : stack(std::vector<T>()) {};

    /// Push a new value onto the stack.
    inline void push(std::unique_ptr<T> val) { stack.push_back(std::move(val)); }

    /// Pop a value off the stack.
    [[nodiscard]]
    inline std::unique_ptr<T> pop() {
      std::unique_ptr<T> val = stack.back();
      stack.pop_back();
      return std::move(val);
    }

    /// Peek at the top of the stack.
    [[nodiscard]]
    inline std::unique_ptr<T> peek() { return std::move(stack.back()); }

    /// Check if the stack is empty.
    [[nodiscard]]
    inline bool is_empty() const { return stack.empty(); }
};

#endif  // STATIMC_STACK_H
