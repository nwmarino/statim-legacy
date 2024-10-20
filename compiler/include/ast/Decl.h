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
#include "../token/Token.h"

/// Base class for all AST declarations.
class Decl
{
public:
  virtual ~Decl() = default;
  virtual void pass(ASTVisitor *visitor) = 0;
  virtual const Metadata get_meta() const = 0;
  virtual bool is_priv() const = 0;
  virtual void set_priv() = 0;
  virtual void set_pub() = 0;
  virtual const std::string to_string() = 0;
};


/// ScopedDecl - Base class for declarations with that contain a scope.
class ScopedDecl : public Decl
{
protected:
  std::shared_ptr<Scope> scope;

public:
  ScopedDecl(std::shared_ptr<Scope> scope) : scope(scope) {};
  std::shared_ptr<Scope> get_scope() const { return scope; }
};


/// NamedDecl - Base class for declarations with a name.
class NamedDecl : public Decl
{
protected:
  const std::string name;

public:
  NamedDecl(const std::string &name) : name(name) {};
  inline const std::string get_name() const { return name; }
};


/// TypeDecl - Base class for type declarations.
///
/// A type declaration is a declaration that creates a type, like a struct.
/// This should not be mistaken for declarations that are typed, like variables.
class TypeDecl : public NamedDecl
{
protected:
  const DefinedType *T;

public:
  TypeDecl(const std::string &name, const DefinedType *T) : NamedDecl(name), T(T) {};
  inline const DefinedType* get_type() const { return T; }
  inline void set_type(const DefinedType *T) { this->T = T; }
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
  std::vector<NamedDecl *> decls;

public:
  Scope(std::shared_ptr<Scope> parent, struct ScopeContext ctx) 
    : parent(parent), ctx(ctx), decls(){};

  /// Add a declaration to this scope.
  inline void add_decl(NamedDecl *d) { decls.push_back(d); }

  /// Delete a declaration from this scope.
  inline void del_decl(NamedDecl *d) { decls.erase(std::remove(decls.begin(), decls.end(), d), decls.end()); }

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
  inline NamedDecl *get_decl(const std::string &name) {
    // check that the identifier is not reserved
    if (is_reserved_ident(name)) {
      return nullptr;
    }

    for (NamedDecl *d : decls) { 
      if (d->get_name() == name) {
        return d;
      }
    }
    if (parent) {
      return parent->get_decl(name);
    }
    return nullptr;
  }

  /// Get all declarations in this scope.
  [[nodiscard]]
  inline const std::vector<NamedDecl *> get_decls() const { return decls; }
  
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
    std::string result = "Scope\n";
    for (NamedDecl *d : decls) {
      result += d->get_name() + '\n';
    }
    return result;
  }

  /// Returns a string respresentation of this scope tree with an identifier.
  [[nodiscard]]
  const std::string to_string(const std::string &id) {
    std::string result = "Scope: " + id + '\n';
    for (NamedDecl *d : decls) {
      result += d->get_name() + '\n';
    }
    return result;
  }
};


/// Function declaration related classes.
///
/// Functions hold a list of parameters and a body.

/// Class for function parameters.
class ParamVarDecl final : public NamedDecl
{
private:
  const Type *T;
  const Metadata meta;

public:
  ParamVarDecl(const std::string &name, Type *T, const Metadata &meta) 
    : NamedDecl(name), T(T), meta(meta) {};
  void pass(ASTVisitor *visitor) override { visitor->visit(this); }
  inline const Metadata get_meta() const override { return meta; }
  inline const Type* get_type() const { return T; }
  inline void set_type(const Type *T) { this->T = T; }
  inline bool is_priv() const override { return false; }
  inline void set_priv() override {}
  inline void set_pub() override {}

  /// Returns a string representation of this parameter.
  [[nodiscard]]
  const std::string to_string() override;
};


/// Class for function definitions and declarations.
class FunctionDecl final : public NamedDecl, public ScopedDecl
{
private:
  const Type *T;
  const Metadata meta;
  std::vector<std::unique_ptr<ParamVarDecl>> params;
  std::unique_ptr<Stmt> body;
  bool priv;

public:
  FunctionDecl(const std::string &name, Type *T, std::vector<std::unique_ptr<ParamVarDecl>> params, const Metadata &meta) 
    : NamedDecl(name), ScopedDecl(nullptr), T(T), meta(meta), params(std::move(params)), body(nullptr), priv(name == "main" ? true : false) {};
  FunctionDecl(const std::string &name, Type *T, std::vector<std::unique_ptr<ParamVarDecl>> params, std::unique_ptr<Stmt> body, 
    std::shared_ptr<Scope> scope, const Metadata &meta)
    : NamedDecl(name), ScopedDecl(scope), T(T), meta(meta), params(std::move(params)), body(std::move(body)), priv(name == "main" ? true : false) {};
  void pass(ASTVisitor *visitor) override { visitor->visit(this); }
  llvm::Function *codegen() const;
  inline const Type* get_type() const { return T; }
  inline void set_type(const Type *T) { this->T = T; }
  inline int get_num_params() const { return params.size(); }
  inline const Metadata get_meta() const override { return meta; }

  /// Returns true if this function declaration has a body.
  inline bool has_body() const { return body != nullptr; }

  /// Returns true if this is the entry function main.
  inline bool is_main() const { return name == "main"; }

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
class TraitDecl final : public NamedDecl
{
private:
  std::vector<std::unique_ptr<FunctionDecl>> decls;
  const Metadata meta;
  bool priv;

public:
  TraitDecl(const std::string &name, const Metadata &meta) : NamedDecl(name), decls(), meta(meta), priv(false) {};
  TraitDecl(const std::string &name, std::vector<std::unique_ptr<FunctionDecl>> decls, const Metadata &meta)
    : NamedDecl(name), decls(std::move(decls)), meta(meta), priv(false) {};
  void pass(ASTVisitor *visitor) override { visitor->visit(this); }
  inline const Metadata get_meta() const override { return meta; }
  
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
class EnumVariantDecl final : public NamedDecl
{
private:
  const Metadata meta;

public:
  EnumVariantDecl(const std::string &name, const Metadata &meta) : NamedDecl(name), meta(meta) {};
  void pass(ASTVisitor *visitor) override { visitor->visit(this); }
  inline bool is_priv() const override { return false; }
  inline void set_priv() override {}
  inline void set_pub() override {}
  inline const Metadata get_meta() const override { return meta; }

  /// Returns a string representation of this enum variant.
  [[nodiscard]]
  const std::string to_string() override;
};


/// Class for enum declarations.
class EnumDecl final : public TypeDecl
{
private:
  std::vector<std::unique_ptr<EnumVariantDecl>> variants;
  const Metadata meta;
  bool priv;

public:
  EnumDecl(const std::string &name, const Metadata &meta) 
    : TypeDecl(name, nullptr), variants(), meta(meta), priv(false) {};
  EnumDecl(const std::string &name, std::vector<std::unique_ptr<EnumVariantDecl>> variants, const Metadata &meta)
    : TypeDecl(name, nullptr), variants(std::move(variants)), meta(meta), priv(false) {};
  void pass(ASTVisitor *visitor) override { visitor->visit(this); }
  inline const Metadata get_meta() const override { return meta; }

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
  inline const Metadata get_meta() const override { return meta; }

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
  inline const std::string get_struct_name() const { return _struct; }

  /// Returns true if this is a trait implementation.
  inline bool is_trait() const { return is_trait_impl; }

  /// Returns a string representation of this implementation declaration.
  const std::string to_string() override;
};


/// Struct declaration related classes.
///
/// Structs hold a list of fields and methods.

/// Class for struct fields.
class FieldDecl final : public NamedDecl
{
private:
  const Type *T;
  const Metadata meta;
  bool priv;

public:
  FieldDecl(const std::string &name, const Type *T, const Metadata &meta) 
    : NamedDecl(name), T(T), meta(meta), priv(false) {};
  void pass(ASTVisitor *visitor) override { visitor->visit(this); }
  inline const Type* get_type() const { return T; }
  inline void set_type(const Type *T) { this->T = T; }
  inline const Metadata get_meta() const override { return meta; }

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
  std::vector<std::unique_ptr<FieldDecl>> fields;
  std::shared_ptr<Scope> scope;
  std::vector<std::string> impls;
  bool priv;
  const Metadata meta;

public:
  StructDecl(const std::string &name, std::vector<std::unique_ptr<FieldDecl>> fields, std::shared_ptr<Scope> scope, const Metadata &meta)
    : ScopedDecl(scope), TypeDecl(name, nullptr), fields(std::move(fields)), priv(false), impls(), meta(meta) {};
  void pass(ASTVisitor *visitor) override { visitor->visit(this); }
  inline const Metadata get_meta() const override { return meta; }

  /// Returns true if this function declaration is private.
  inline bool is_priv() const override { return priv; }

  /// Set this function declaration as private.
  inline void set_priv() override { priv = true; }

  // Set this function declaration as public.
  inline void set_pub() override { priv = false; }

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
class VarDecl final : public NamedDecl
{
private:
  const Type *T;
  std::unique_ptr<Expr> expr;
  const Metadata meta;
  bool mut;
  bool rune;
  

public:
  VarDecl(const std::string &name, const Type *T, std::unique_ptr<Expr> expr, bool mut, bool rune, const Metadata &meta)
    : NamedDecl(name), T(T), expr(std::move(expr)), meta(meta), mut(mut), rune(rune) {};
  VarDecl(const std::string &name, const Type *T, bool mut, bool rune, const Metadata &meta)
    : NamedDecl(name), T(T), expr(nullptr), meta(meta), mut(mut), rune(rune) {};
  void pass(ASTVisitor *visitor) override { visitor->visit(this); }
  inline const Type* get_type() const { return T; }
  inline void set_type(const Type *T) { this->T = T; }
  inline bool is_priv() const override { return false; }
  inline void set_priv() override {}
  inline void set_pub() override {}
  inline const Metadata get_meta() const override { return meta; }

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
