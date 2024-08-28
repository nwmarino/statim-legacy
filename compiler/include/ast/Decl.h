#ifndef DECL_STATIMC_H
#define DECL_STATIMC_H

/// Declaration AST nodes.
/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <algorithm>
#include <memory>
#include <vector>

class Decl;

/// Base class for all AST declarations.
class Decl
{
  public:
    virtual ~Decl() = default;
    virtual const std::string to_string(int n) = 0;
};


/// Context about a scope.
struct ScopeContext
{
  /// If this scope is nested in a package.
  bool is_pkg_scope;

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
    std::shared_ptr<Scope> parent;
    struct ScopeContext ctx;
    std::vector<Decl *> decls;

  public:
    /// Constructor for a scope.
    Scope(std::shared_ptr<Scope> parent, struct ScopeContext ctx)
      : parent(parent), ctx(ctx), decls() {};

    /// Add a declaration to this scope.
    inline void add_decl(Decl *d) { decls.push_back(d); }

    /// Delete a declaration from this scope.
    inline void del_decl(Decl *d) { decls.erase(std::remove(decls.begin(), decls.end(), d), decls.end()); }

    /// Get the direct parent scope, if it exists.
    [[nodiscard]]
    inline std::shared_ptr<Scope> get_parent() { 
      return parent;
    }

    /// Get the closest function scope, if it exists.
    [[nodiscard]]
    inline std::shared_ptr<Scope> get_fn_scope() {
      std::shared_ptr<Scope> p = parent;
      while (p != nullptr && !p->is_func_scope()) {
        p = p->parent;
      }
      return p;
    }

    /// Determine if this scope belongs to a package.
    [[nodiscard]]
    inline bool is_pkg_scope() const { return ctx.is_pkg_scope; }

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

    /// Returns a string representation of this scope tree.
    [[nodiscard]]
    const std::string to_string(int n);
};


#include "Stmt.h"

/// Function declaration related classes.
///
/// Functions hold a list of parameters and a body.

/// Class for function parameters.
class FunctionParam
{
  private:
    const std::string name;
    const std::string type;

  public:
    /// Basic constructor for parameters.
    FunctionParam(const std::string &name, const std::string &type)
      : name(name), type(type) {};

    /// Gets the name of this parameter.
    [[nodiscard]]
    inline const std::string get_name() const { return name; }

    /// Gets the type of this parameter.
    [[nodiscard]]
    inline const std::string get_type() const { return type; }
};

/// Class for function definitions and declarations.
class FunctionDecl : public Decl
{
  private:
    const std::string name;
    const std::string ret_type;
    std::vector<FunctionParam> params;
    std::unique_ptr<Stmt> body;
    std::shared_ptr<Scope> scope;
    bool priv;

  public:
    /// Constructor for function declarations with no body.
    FunctionDecl(const std::string &name, const std::string &ret_type, std::vector<FunctionParam> params)
      : name(name), ret_type(ret_type), params(std::move(params)), body(nullptr), priv(false) {};

    /// Constructor for function declarations with a body.
    FunctionDecl(const std::string &name, const std::string &ret_type, std::vector<FunctionParam> params, std::unique_ptr<Stmt> body, std::shared_ptr<Scope> scope)
      : name(name), ret_type(ret_type), params(std::move(params)), body(std::move(body)), scope(scope), priv(false) {};
    
    /// Returns true if this function declaration has a body.
    [[nodiscard]]
    inline bool has_body() const { return body != nullptr; }

    /// Returns true if this is the entry function main.
    [[nodiscard]]
    inline bool is_main() const { return name == "main"; }

    /// Gets the name of this function declaration.
    [[nodiscard]]
    inline const std::string get_name() const { return name; }

    /// Gets the return type of this function declaration.
    [[nodiscard]]
    inline const std::string get_ret_type() const { return ret_type; }

    /// Gets the parameters of this function declaration.
    [[nodiscard]]
    inline const std::vector<FunctionParam> get_params() const { return params; }

    /// Returns true if this function declaration is private.
    [[nodiscard]]
    inline bool is_priv() const { return priv; }

    /// Set this function declaration as private.
    inline void set_priv() { priv = true; }

    // Set this function declaration as public.
    inline void set_pub() { priv = false; }

    /// Returns a string representation of this function declaration.
    [[nodiscard]]
    const std::string to_string(int n);
};


/// Trait declaration related classes.
///
/// Trait declarations hold a list of function prototypes.

/// Class for trait declarations.
class TraitDecl : public Decl
{
  private:
    const std::string name;
    std::vector<std::unique_ptr<FunctionDecl>> decls;
    bool priv;

  public:
    /// Constructor for trait declarations with no function declarations.
    TraitDecl(const std::string &name)
      : name(name), decls(), priv(false) {};

    /// Constructor for trait declarations with function declarations.
    TraitDecl(const std::string &name, std::vector<std::unique_ptr<FunctionDecl>> decls)
      : name(name), decls(std::move(decls)), priv(false) {};

    /// Gets the name of this trait declaration.
    [[nodiscard]]
    inline const std::string get_name() const { return name; }

    /// Returns true if this function declaration is private.
    [[nodiscard]]
    inline bool is_priv() const { return priv; }

    /// Set this function declaration as private.
    inline void set_priv() { priv = true; }

    // Set this function declaration as public.
    inline void set_pub() { priv = false; }

    /// Returns a string representation of this trait declaration.
    [[nodiscard]]
    const std::string to_string(int n);
};


/// Enum declaration related classes.
///
/// Enums hold a list of variants which are expanded into constants.

/// Class for enum variants.
class EnumVariant
{
  private:
    const std::string name;

  public:
    /// Basic constructor for enum variants.
    EnumVariant(const std::string &name)
      : name(name) {};

    /// Gets the name of this enum variant.
    [[nodiscard]]
    inline const std::string get_name() const { return name; }

    /// Returns a string representation of this enum variant.
    [[nodiscard]]
    const std::string to_string(int n);
};

/// Class for enum declarations.
class EnumDecl : public Decl
{
  private:
    const std::string name;
    std::vector<EnumVariant> variants;
    bool priv;

  public:
    /// Constructor for enum declarations with no variants.
    EnumDecl(const std::string &name)
      : name(name), variants(), priv(false) {};

    /// Constructor for enum declarations with variants.
    EnumDecl(const std::string &name, std::vector<EnumVariant> variants)
      : name(name), variants(std::move(variants)), priv(false) {};

    /// Gets the name of this enum declaration.
    [[nodiscard]]
    inline const std::string get_name() const { return name; }

    /// Gets the variants of this enum declaration.
    [[nodiscard]]
    inline const std::vector<EnumVariant> get_variants() const { return variants; }

    /// Returns true if this function declaration is private.
    [[nodiscard]]
    inline bool is_priv() const { return priv; }

    /// Set this function declaration as private.
    inline void set_priv() { priv = true; }

    // Set this function declaration as public.
    inline void set_pub() { priv = false; }

    /// Returns a string representation of this enum declaration.
    [[nodiscard]]
    const std::string to_string(int n);
};


/// Implementation declaration related classes.
///
/// Impls apply abstract declarations to structs.

/// Class for implementation declarations.
class ImplDecl : public Decl
{
  private:
    const std::string abstract_name;
    const std::string struct_name;
    std::vector<std::unique_ptr<FunctionDecl>> methods;

  public:
    /// Constructor for implementation declarations with no methods.
    ImplDecl(const std::string &abstract_name, const std::string &struct_name)
      : abstract_name(abstract_name), struct_name(struct_name), methods() {};

    /// Constructor for implementation declarations with methods.
    ImplDecl(const std::string &abstract_name, const std::string &struct_name, std::vector<std::unique_ptr<FunctionDecl>> methods)
      : abstract_name(abstract_name), struct_name(struct_name), methods(std::move(methods)) {};

    /// Gets the name of the abstract declaration of this implementation declaration.
    [[nodiscard]]
    inline const std::string get_name() const { return abstract_name; }

    /// Gets the name of the struct of this implementation declaration.
    [[nodiscard]]
    inline const std::string get_struct_name() const { return struct_name; }

    /// Returns a string representation of this implementation declaration.
    [[nodiscard]]
    const std::string to_string(int n);
};


/// Struct declaration related classes.
///
/// Structs hold a list of fields and methods.

/// Class for struct fields.
class FieldDecl : public Decl
{
  private:
    const std::string name;
    const std::string type;
    bool priv;

  public:
    /// Basic constructor for struct fields.
    FieldDecl(const std::string &name, const std::string &type)
      : name(name), type(type), priv(false) {};

    /// Gets the name of this struct fields.
    [[nodiscard]]
    inline const std::string get_name() const { return name; }

    /// Gets the type of this struct fields.
    [[nodiscard]]
    inline const std::string get_type() const { return type; }

    /// Returns true if this function declaration is private.
    [[nodiscard]]
    inline bool is_priv() const { return priv; }

    /// Set this function declaration as private.
    inline void set_priv() { priv = true; }

    // Set this function declaration as public.
    inline void set_pub() { priv = false; }

    /// Returns a string representation of this struct fields.
    [[nodiscard]]
    const std::string to_string(int n);
};

/// Class for struct declarations.
class StructDecl : public Decl
{
  private:
    const std::string name;
    std::vector<std::unique_ptr<FieldDecl>> fields;
    std::shared_ptr<Scope> scope;
    bool priv;

  public:
    /// Basic constructor for struct declarations.
    StructDecl(const std::string &name, std::vector<std::unique_ptr<FieldDecl>> fields, std::shared_ptr<Scope> scope)
      : name(name), fields(std::move(fields)), scope(scope), priv(false) {};

    /// Gets the name of this struct declaration.
    [[nodiscard]]
    inline const std::string get_name() const { return name; }

    /// Returns true if this function declaration is private.
    [[nodiscard]]
    inline bool is_priv() const { return priv; }

    /// Set this function declaration as private.
    inline void set_priv() { priv = true; }

    // Set this function declaration as public.
    inline void set_pub() { priv = false; }

    /// Returns a string representation of this struct declaration.
    [[nodiscard]]
    const std::string to_string(int n);
};


/// Class for variable declarations.
class VarDecl : public Decl
{
  private:
    const std::string name;
    const std::string type;
    std::unique_ptr<Expr> expr;
    bool mut;

  public:
    /// Constructor for variable declarations with an expression.
    VarDecl(const std::string &name, const std::string &type, std::unique_ptr<Expr> expr, bool mut)
      : name(name), type(type), expr(std::move(expr)), mut(mut) {};

    /// Constructor for variable declarations without an expression.
    VarDecl(const std::string &name, const std::string &type, bool mut)
      : name(name), type(type), expr(nullptr), mut(mut) {};

    /// Gets the name of this variable declaration.
    [[nodiscard]]
    inline const std::string get_name() const { return name; }

    /// Gets the type of this variable declaration.
    [[nodiscard]]
    inline const std::string get_type() const { return type; }

    /// Returns true if this variable declaration has an expression.
    [[nodiscard]]
    inline bool has_expr() const { return expr != nullptr; }

    /// Gets the expression of this variable declaration.
    [[nodiscard]]
    inline std::unique_ptr<Expr> &get_expr() { return expr; }

    /// Determine if this variable declaration is mutable.
    [[nodiscard]]
    inline bool is_mut() const { return mut; }

    /// Returns a string representation of this variable declaration.
    [[nodiscard]]
    const std::string to_string(int n);
};

#endif  // DECL_STATIMC_H
