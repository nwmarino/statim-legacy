#ifndef DECL_STATIMC_H
#define DECL_STATIMC_H

/// Declaration AST nodes.
/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "Stmt.h"

/// Base class for all AST declarations.
class Decl
{
  public:
    virtual ~Decl() = default;
    const virtual std::string to_string(int n) = 0;
};


/// Function declaration related classes. ///

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

  public:
    /// Constructor for function declarations with no parameters and no body.
    FunctionDecl(const std::string &name, const std::string &ret_type)
      : name(name), ret_type(ret_type), params(), body(nullptr) {};

    /// Constructor for function declarations with parameters and no body.
    FunctionDecl(const std::string &name, const std::string &ret_type, std::vector<FunctionParam> params)
      : name(name), ret_type(ret_type), params(std::move(params)), body(nullptr) {};

    /// Constructor for function declarations with no parameters a body.
    FunctionDecl(const std::string &name, const std::string &ret_type, std::unique_ptr<Stmt> body)
      : name(name), ret_type(ret_type), params(), body(std::move(body)) {};

    /// Constructor for function declarations with parameters and a body.
    FunctionDecl(const std::string &name, const std::string &ret_type, std::vector<FunctionParam> params, std::unique_ptr<Stmt> body)
      : name(name), ret_type(ret_type), params(std::move(params)), body(std::move(body)) {};
    
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

    /// Returns a string representation of this function declaration.
    [[nodiscard]]
    const std::string to_string(int n);
};


/// Abstract declaration related classes. ///

/// Class for abstract declarations.
class AbstractDecl : public Decl
{
  private:
    const std::string name;
    std::vector<std::unique_ptr<FunctionDecl>> decls;

  public:
    /// Constructor for abstract declarations with no function declarations.
    AbstractDecl(const std::string &name)
      : name(name), decls() {};

    /// Constructor for abstract declarations with function declarations.
    AbstractDecl(const std::string &name, std::vector<std::unique_ptr<FunctionDecl>> decls)
      : name(name), decls(std::move(decls)) {};

    /// Gets the name of this abstract declaration.
    [[nodiscard]]
    inline const std::string get_name() const { return name; }

    /// Gets the function declarations of this abstract declaration.
    [[nodiscard]]
    inline const std::vector<std::unique_ptr<FunctionDecl>> get_decls() const { return decls; }

    /// Returns a string representation of this abstract declaration.
    [[nodiscard]]
    const std::string to_string(int n);
};


/// Enum declaration related classes. ///

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
};

/// Class for enum declarations.
class EnumDecl : public Decl
{
  private:
    const std::string name;
    std::vector<EnumVariant> variants;

  public:
    /// Constructor for enum declarations with no variants.
    EnumDecl(const std::string &name)
      : name(name), variants() {};

    /// Constructor for enum declarations with variants.
    EnumDecl(const std::string &name, std::vector<EnumVariant> variants)
      : name(name), variants(std::move(variants)) {};

    /// Gets the name of this enum declaration.
    [[nodiscard]]
    inline const std::string get_name() const { return name; }

    /// Gets the variants of this enum declaration.
    [[nodiscard]]
    inline const std::vector<EnumVariant> get_variants() const { return variants; }

    /// Returns a string representation of this enum declaration.
    [[nodiscard]]
    const std::string to_string(int n);
};


/// Implementation declaration related classes. ///

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
    inline const std::string get_abstract_name() const { return abstract_name; }

    /// Gets the name of the struct of this implementation declaration.
    [[nodiscard]]
    inline const std::string get_struct_name() const { return struct_name; }

    /// Gets the methods of this implementation declaration.
    [[nodiscard]]
    inline const std::vector<std::unique_ptr<FunctionDecl>> get_methods() const { return methods; }

    /// Returns a string representation of this implementation declaration.
    [[nodiscard]]
    const std::string to_string(int n);
};


/// Struct declaration related classes. ///

/// Class for struct members.
class StructMember
{
  private:
    const std::string name;
    const std::string type;

  public:
    /// Basic constructor for struct members.
    StructMember(const std::string &name, const std::string &type)
      : name(name), type(type) {};

    /// Gets the name of this struct member.
    [[nodiscard]]
    inline const std::string get_name() const { return name; }

    /// Gets the type of this struct member.
    [[nodiscard]]
    inline const std::string get_type() const { return type; }
};

/// Class for struct declarations.
class StructDecl : public Decl
{
  private:
    const std::string name;
    std::vector<StructMember> members;
    std::vector<FunctionDecl> methods;

  public:
    /// Constructor for struct declarations with no methods and no members.
    StructDecl(const std::string &name)
      : name(name), members(), methods() {};

    /// Constructor for struct declarations with methods and no members.
    StructDecl(const std::string &name, std::vector<FunctionDecl> methods)
      : name(name), members(), methods(std::move(methods)) {};

    /// Constructor for struct declarations with members and no methods.
    StructDecl(const std::string &name, std::vector<StructMember> members)
      : name(name), members(std::move(members)), methods() {};

    /// Constructor for struct declarations with members.and methods.
    StructDecl(const std::string &name, std::vector<StructMember> members, std::vector<FunctionDecl> methods)
      : name(name), members(std::move(members)), methods(std::move(methods)) {};

    /// Gets the name of this struct declaration.
    [[nodiscard]]
    inline const std::string get_name() const { return name; }

    /// Gets the members of this struct declaration.
    [[nodiscard]]
    inline const std::vector<StructMember> get_members() const { return members; }

    /// Gets the methods of this struct declaration.
    [[nodiscard]]
    inline const std::vector<FunctionDecl> get_methods() const { return methods; }

    /// Returns a string representation of this struct declaration.
    [[nodiscard]]
    const std::string to_string(int n);
};

#endif  // DECL_STATIMC_H
