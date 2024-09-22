#ifndef DECL_STATIMC_H
#define DECL_STATIMC_H

/// Declaration AST nodes.
/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

#include "Expr.h"
#include "../sema/ASTVisitor.h"

class Stmt;

/// Base class for all AST declarations.
class Decl
{
public:
  virtual ~Decl() = default;
  virtual void pass(ASTVisitor *visitor) = 0;
  virtual const std::string get_name() const = 0;
  virtual const std::string to_string() = 0;
  virtual bool is_priv() const = 0;
  virtual void set_priv() = 0;
  virtual void set_pub() = 0;
  virtual const Metadata get_meta() const = 0;
};


/// ScopedDecl - Base class for declarations with that contain a scope.
class ScopedDecl : public Decl
{
private:
  std::shared_ptr<Scope> scope;

public:
  virtual ~ScopedDecl() = default;
  virtual std::shared_ptr<Scope> get_scope() const = 0;
  virtual const Metadata get_meta() const = 0;
};


/// TypeDecl - Base class for type declarations.
class TypeDecl : public Decl
{
private:
  const Type *T;

public:
  virtual ~TypeDecl() = default;
  virtual void pass(ASTVisitor *visitor) = 0;
  virtual const Type* get_type() const = 0;
  virtual void set_type(const Type *T) = 0;
  virtual const std::string get_name() const = 0;
  virtual const std::string to_string() = 0;
  virtual const Metadata get_meta() const = 0;
};


/// Context about a scope.
struct ScopeContext final
{
public:
  /// If this scope is nested in a crate.
  bool is_crate_scope;

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
class Scope final 
{
private:
  std::shared_ptr<Scope> parent;
  struct ScopeContext ctx;
  std::vector<Decl *> decls;

public:
  Scope(std::shared_ptr<Scope> parent, struct ScopeContext ctx) : parent(parent), ctx(ctx), decls() {};

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

  /// Get a declaration by its name, if it exists.
  [[nodiscard]]
  inline Decl *get_decl(const std::string &name) {
    // check that the identifier is not reserved
    if (is_reserved_ident(name)) {
      return nullptr;
    }

    for (Decl *d : decls) {
      if (d->get_name() == name) {
        return d;
      }
    }
    if (parent) {
      return parent->get_decl(name);
    }
    return nullptr;
  }
  
  /// Determine if this scope belongs to a crate.
  [[nodiscard]]
  inline bool is_crate_scope() const { return ctx.is_crate_scope; }

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
  const std::string to_string() {
    std::string result;
    for (Decl *d : decls) {
      result += d->to_string() + '\n';
    }
    return result;
  }
};


/// Function declaration related classes.
///
/// Functions hold a list of parameters and a body.

/// Class for function parameters.
class ParamVarDecl final : public Decl
{
private:
  const std::string name;
  const Type *T;
  const Metadata meta;

public:
  ParamVarDecl(const std::string &name, Type *T, const Metadata &meta) 
    : name(name), T(T), meta(meta){};
  void pass(ASTVisitor *visitor) override { visitor->visit(this); }
  inline const Type* get_type() const { return T; }
  inline void set_type(const Type *T) { this->T = T; }
  inline bool is_priv() const override { return false; }
  inline void set_priv() override {}
  inline void set_pub() override {}
  const Metadata get_meta() const override { return meta; }

  /// Gets the name of this parameter.
  [[nodiscard]]
  inline const std::string get_name() const override { return name; }

  /// Returns a string representation of this parameter.
  [[nodiscard]]
  const std::string to_string() override;
};


/// Class for function definitions and declarations.
class FunctionDecl final : public ScopedDecl
{
private:
  const std::string name;
  const Type *T;
  std::vector<std::unique_ptr<ParamVarDecl>> params;
  std::unique_ptr<Stmt> body;
  std::shared_ptr<Scope> scope;
  bool priv;
  const Metadata meta;

public:
  FunctionDecl(const std::string &name, Type *T, std::vector<std::unique_ptr<ParamVarDecl>> params, 
    const Metadata &meta) : name(name), T(T), params(std::move(params)), body(nullptr), priv(false), meta(meta){};
  FunctionDecl(const std::string &name, Type *T, 
    std::vector<std::unique_ptr<ParamVarDecl>> params, std::unique_ptr<Stmt> body, 
    std::shared_ptr<Scope> scope, const Metadata &meta)
    : name(name), T(T), params(std::move(params)), body(std::move(body)), scope(scope), priv(false), meta(meta){};
  void pass(ASTVisitor *visitor) override { visitor->visit(this); }
  inline const Type* get_type() const { return T; }
  inline void set_type(const Type *T) { this->T = T; }
  inline int get_num_params() const { return params.size(); }
  const Metadata get_meta() const override { return meta; }

  /// Returns true if this function declaration has a body.
  inline bool has_body() const { return body != nullptr; }

  /// Returns true if this is the entry function main.
  inline bool is_main() const { return name == "main"; }

  /// Gets the name of this function declaration.
  inline const std::string get_name() const override { return name; }

  /// Returns true if the function has a parameters, and false otherwise.
  inline bool has_params() const { return !params.empty(); }

  /// Returns the parameters of this function declaration.
  inline const std::vector<ParamVarDecl *> get_params() const {
    std::vector<ParamVarDecl *> params = {};
    for (const std::unique_ptr<ParamVarDecl> &p : this->params) {
      params.push_back(p.get());
    }
    return params;
  }

  /// Returns the body of this function declaration.
  inline Stmt *get_body() const { return body.get(); }

  /// Get the scope of this function declaration.
  inline std::shared_ptr<Scope> get_scope() const override { return scope; }

  /// Returns true if this function declaration is private.
  inline bool is_priv() const override { return priv; }

  /// Set this function declaration as private.
  inline void set_priv() override { priv = true; }

  // Set this function declaration as public.
  inline void set_pub() override { priv = false; }

  /// Returns a string representation of this function declaration.
  const std::string to_string() override;
};


/// Trait declaration related classes.
///
/// Trait declarations hold a list of function prototypes.

/// Class for trait declarations.
class TraitDecl final : public Decl
{
private:
  const std::string name;
  std::vector<std::unique_ptr<FunctionDecl>> decls;
  bool priv;
  const Metadata meta;

public:
  TraitDecl(const std::string &name, const Metadata &meta) : name(name), decls(), priv(false), meta(meta){};
  TraitDecl(const std::string &name, std::vector<std::unique_ptr<FunctionDecl>> decls, const Metadata &meta)
    : name(name), decls(std::move(decls)), priv(false), meta(meta){};
  void pass(ASTVisitor *visitor) override { visitor->visit(this); }
  const Metadata get_meta() const override { return meta; }

  /// Gets the name of this trait declaration.
  [[nodiscard]]
  inline const std::string get_name() const override { return name; }

  // Returns the expected method behaviour of this trait declaration.
  [[nodiscard]]
  inline const std::vector<FunctionDecl *> get_decls() const {
    std::vector<FunctionDecl *> decls = {};
    for (const std::unique_ptr<FunctionDecl> &d : this->decls) {
      decls.push_back(d.get());
    }
    return decls;
  }

  /// Returns true if this function declaration is private.
  [[nodiscard]]
  inline bool is_priv() const override { return priv; }

  /// Set this function declaration as private.
  inline void set_priv() override { priv = true; }

  // Set this function declaration as public.
  inline void set_pub() override { priv = false; }

  /// Returns a string representation of this trait declaration.
  const std::string to_string() override;
};


/// Enum declaration related classes.
///
/// Enums hold a list of variants which are expanded into constants.

/// Class for enum variants.
class EnumVariantDecl final : public Decl
{
private:
  const std::string name;
  const Metadata meta;

public:
  EnumVariantDecl(const std::string &name, const Metadata &meta) : name(name), meta(meta){};
  void pass(ASTVisitor *visitor) override { visitor->visit(this); }
  inline bool is_priv() const override { return false; }
  inline void set_priv() override {}
  inline void set_pub() override {}
  const Metadata get_meta() const override { return meta; }

  /// Gets the name of this enum variant.
  [[nodiscard]]
  inline const std::string get_name() const override { return name; }

  /// Returns a string representation of this enum variant.
  [[nodiscard]]
  const std::string to_string() override;
};


/// Class for enum declarations.
class EnumDecl final : public TypeDecl
{
private:
  const std::string name;
  std::vector<std::unique_ptr<EnumVariantDecl>> variants;
  const Type *T;
  bool priv;
  const Metadata meta;

public:
  EnumDecl(const std::string &name, const Metadata &meta) 
    : name(name), variants(), priv(false), meta(meta){};
  EnumDecl(const std::string &name, std::vector<std::unique_ptr<EnumVariantDecl>> variants, const Metadata &meta)
    : name(name), variants(std::move(variants)), priv(false), meta(meta){};
  void pass(ASTVisitor *visitor) override { visitor->visit(this); }
  inline const Type* get_type() const override { return T; }
  inline void set_type(const Type *T) override { this->T = T; }
  const Metadata get_meta() const override { return meta; }

  /// Gets the name of this enum declaration.
  inline const std::string get_name() const override { return name; }

  /// Gets the variants of this enum declaration.
  inline const std::vector<EnumVariantDecl *> get_variants() const {
    std::vector<EnumVariantDecl *> variants = {};
    for (const std::unique_ptr<EnumVariantDecl> &v : this->variants) {
      variants.push_back(v.get());
    }
    return variants;
  }

  /// Returns true if this function declaration is private.
  inline bool is_priv() const override { return priv; }

  /// Set this function declaration as private.
  inline void set_priv() override { priv = true; }

  // Set this function declaration as public.
  inline void set_pub() override { priv = false; }

  /// Returns a string representation of this enum declaration.
  const std::string to_string() override;
};


/// Implementation declaration related classes.
///
/// Impls apply abstract declarations to structs.

/// Class for implementation declarations.
class ImplDecl final : public Decl
{
private:
  const std::string _trait;
  const std::string _struct;
  std::vector<std::unique_ptr<FunctionDecl>> methods;
  bool is_trait_impl;
  const Metadata meta;

public:
  ImplDecl(const std::string &_trait, const std::string &_struct, 
    std::vector<std::unique_ptr<FunctionDecl>> methods, const Metadata &meta)
    : _trait(_trait), _struct(_struct), methods(std::move(methods)), 
    is_trait_impl(_trait == "" ? false : true), meta(meta){};
  void pass(ASTVisitor *visitor) override { visitor->visit(this); }
  inline bool is_priv() const override { return false; }
  inline void set_priv() override {}
  inline void set_pub() override {}
  const Metadata get_meta() const override { return meta; }

  /// Returns the methods of this implementation declaration.
  inline const std::vector<FunctionDecl *> get_methods() const {
    std::vector<FunctionDecl *> methods = {};
    for (const std::unique_ptr<FunctionDecl> &m : this->methods) {
      methods.push_back(m.get());
    }
    return methods;
  }

  /// Returns a method by its name, if it exists.
  inline FunctionDecl *get_method(const std::string &name) {
    for (const std::unique_ptr<FunctionDecl> &m : methods) {
      if (m->get_name() == name) {
        return m.get();
      }
    }
    return nullptr;
  }

  /// Returns the name of the trait this declaration implements, or an empty string otherwise.
  inline const std::string trait() const { return is_trait() ? _trait : ""; }

  /// Gets the name of the target struct of this implementation declaration.
  inline const std::string get_name() const override { return _struct; }

  /// Returns true if this is a trait implementation.
  inline bool is_trait() const { return is_trait_impl; }

  /// Returns a string representation of this implementation declaration.
  const std::string to_string() override;
};


/// Struct declaration related classes.
///
/// Structs hold a list of fields and methods.

/// Class for struct fields.
class FieldDecl final : public Decl
{
private:
  const std::string name;
  const Type *T;
  bool priv;
  const Metadata meta;

public:
  FieldDecl(const std::string &name, const Type *T, const Metadata &meta) 
    : name(name), T(T), priv(false), meta(meta){};
  void pass(ASTVisitor *visitor) override { visitor->visit(this); }
  inline const Type* get_type() const { return T; }
  inline void set_type(const Type *T) { this->T = T; }
  const Metadata get_meta() const override { return meta; }

  /// Gets the name of this struct fields.
  inline const std::string get_name() const override { return name; }

  /// Returns true if this function declaration is private.
  inline bool is_priv() const override { return priv; }

  /// Set this function declaration as private.
  inline void set_priv() override { priv = true; }

  // Set this function declaration as public.
  inline void set_pub() override { priv = false; }

  /// Returns a string representation of this struct fields.
  const std::string to_string() override;
};


/// Class for struct declarations.
class StructDecl final : public ScopedDecl, public TypeDecl
{
private:
  const std::string name;
  const Type *T;
  std::vector<std::unique_ptr<FieldDecl>> fields;
  std::shared_ptr<Scope> scope;
  std::vector<std::string> impls;
  bool priv;
  const Metadata meta;

public:
  StructDecl(const std::string &name, std::vector<std::unique_ptr<FieldDecl>> fields, 
    std::shared_ptr<Scope> scope, const Metadata &meta) : name(name), T(nullptr), 
    fields(std::move(fields)), scope(scope), priv(false), impls(), meta(meta){};
  void pass(ASTVisitor *visitor) override { visitor->visit(this); }
  inline const Type* get_type() const override { return T; }
  inline void set_type(const Type *T) override { this->T = T; }
  const Metadata get_meta() const override { return meta; }

  /// Gets the name of this struct declaration.
  inline const std::string get_name() const override { return name; }

  /// Returns true if this function declaration is private.
  inline bool is_priv() const override { return priv; }

  /// Set this function declaration as private.
  inline void set_priv() override { priv = true; }

  // Set this function declaration as public.
  inline void set_pub() override { priv = false; }

  /// Get the scope of this struct declaration.
  inline std::shared_ptr<Scope> get_scope() const override { return scope; }

  /// Determine if this struct type has a field by name.
  inline bool has_field(const std::string &name) const {
    return std::find_if(
      fields.begin(), 
      fields.end(), 
      [&name](const std::unique_ptr<FieldDecl> &f) { return f->get_name() == name; }) != fields.end();
  }

  /// Returns the fields of this struct declaration.
  inline const std::vector<FieldDecl *> get_fields() const {
    std::vector<FieldDecl *> fields = {};
    for (const std::unique_ptr<FieldDecl> &f : this->fields) {
      fields.push_back(f.get());
    }
    return fields;
  }

  /// Returns a field by its name, if it exists.
  inline FieldDecl *get_field(const std::string &name) {
    for (const std::unique_ptr<FieldDecl> &f : fields) {
      if (f->get_name() == name) {
        return f.get();
      }
    }
    return nullptr;
  }

  /// Determine if this struct implements a trait.
  inline bool does_impl(const std::string &trait) const {
    return std::find(impls.begin(), impls.end(), trait) != impls.end();
  }

  /// Add a trait implementation to this struct.
  inline void add_impl(const std::string &trait) { impls.push_back(trait); }

  /// Returns a string representation of this struct declaration.
  const std::string to_string() override;
};


/// Class for variable declarations.
class VarDecl final : public Decl
{
private:
  const std::string name;
  const Type *T;
  std::unique_ptr<Expr> expr;
  bool mut;
  bool rune;
  const Metadata meta;

public:
  VarDecl(const std::string &name, const Type *T, std::unique_ptr<Expr> expr, bool mut, bool rune, const Metadata &meta)
    : name(name), T(T), expr(std::move(expr)), mut(mut), rune(rune), meta(meta){};
  VarDecl(const std::string &name, const Type *T, bool mut, bool rune, const Metadata &meta)
    : name(name), T(T), expr(nullptr), mut(mut), rune(rune), meta(meta){};
  void pass(ASTVisitor *visitor) override { visitor->visit(this); }
  inline const Type* get_type() const { return T; }
  inline void set_type(const Type *T) { this->T = T; }
  inline bool is_priv() const override { return false; }
  inline void set_priv() override {}
  inline void set_pub() override {}
  const Metadata get_meta() const override { return meta; }

  /// Gets the name of this variable declaration.
  inline const std::string get_name() const override { return name; }

  /// Returns true if this variable declaration has an expression.
  inline bool has_expr() const { return expr != nullptr; }

  /// Gets the expression of this variable declaration.
  inline std::unique_ptr<Expr> &get_expr() { return expr; }

  /// Determine if this variable declaration is mutable.
  inline bool is_mut() const { return mut; }

  /// Determine if this variable declaration is a rune.
  inline bool is_rune() const { return rune; }

  /// Returns a string representation of this variable declaration.
  const std::string to_string() override;
};

#endif  // DECL_STATIMC_H
