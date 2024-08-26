#ifndef SCOPE_STATIMC_H
#define SCOPE_STATIMC_H

/// Scope used when parsing the AST.
/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <algorithm>

#include "Decl.h"

/// Context about a scope.
class ScopeContext
{
  public:
    /// If this scope is nested in a declaration.
    bool is_decl_scope;

    /// If this scope is nested in a function.
    bool is_func_scope;

    /// If this scope is nested in a struct.
    bool is_struct_scope;

    /// If this scope is nested in a loop.
    bool is_loop_scope;

    /// If this scope is nested in a conditional statement.
    bool is_cond_scope;

    /// If this scope is nested in a compound statement.
    bool is_compound_scope;
};


/// A temporary scope used when parsing the AST.
class Scope
{
  private:
    std::unique_ptr<Scope> parent;
    ScopeContext ctx;
    std::vector<std::unique_ptr<Decl>> decls;

  public:
    /// Constructor for a scope.
    Scope(std::unique_ptr<Scope> parent, ScopeContext ctx)
      : parent(std::move(parent)), ctx(ctx), decls() {};

    /// Add a declaration to this scope.
    inline void add_decl(std::unique_ptr<Decl> decl) { decls.push_back(std::move(decl)); }

    /// Delete a declaration from this scope.
    inline void del_decl(std::unique_ptr<Decl> decl) { decls.erase(std::remove(decls.begin(), decls.end(), decl), decls.end()); }

    /// Get the direct parent scope, if it exists.
    [[nodiscard]]
    inline Scope* get_parent() const { 
      if (parent) {
        return parent.get();
      }
      return nullptr;
    }

    /// Get the closest function scope, if it exists.
    [[nodiscard]]
    inline Scope* get_fn_scope() const {
      Scope* fn_parent = parent.get();
      while (fn_parent != nullptr) {
        if (fn_parent->is_func_scope()) {
          return fn_parent;
        }
        fn_parent = fn_parent->get_parent();
      }
      return nullptr;
    }

    /// Determine if this scope belongs to a declaration.
    [[nodiscard]]
    inline bool is_decl_scope() const { return ctx.is_decl_scope; }

    /// Determine if this scope belongs to a function.
    [[nodiscard]]
    inline bool is_func_scope() const { return ctx.is_func_scope; }

    /// Determine if this scope belongs to a struct.
    [[nodiscard]]
    inline bool is_struct_scope() const { return ctx.is_struct_scope; }

    /// Determine if this scope belongs to a loop.
    [[nodiscard]]
    inline bool is_loop_scope() const { return ctx.is_loop_scope; }

    /// Determine if this scope belongs to a conditional statement.
    [[nodiscard]]
    inline bool is_cond_scope() const { return ctx.is_cond_scope; }

    /// Determine if this scope belongs to a compound statement.
    [[nodiscard]]
    inline bool is_compound_scope() const { return ctx.is_compound_scope; }
};

#endif  // SCOPE_STATIMC_H
