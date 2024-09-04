#ifndef UNIT_STATIMC_H
#define UNIT_STATIMC_H

/// Translation unit related AST nodes.
/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <memory>
#include <string>
#include <vector>

#include "Decl.h"

/// Base class for source units.
class Unit
{
public:
  virtual ~Unit() = default;
  virtual void pass(ASTVisitor *visitor) = 0;
  virtual const std::string to_string() = 0;
};


/// A package is the representation of a modular source file.
class PackageUnit final : public Unit
{
private:
  const std::string name;
  std::vector<std::string> imports;
  std::vector<std::unique_ptr<Decl>> decls;
  std::shared_ptr<Scope> scope;

public:
  PackageUnit(const std::string &name, std::vector<std::string> imports, std::vector<std::unique_ptr<Decl>> decls, std::shared_ptr<Scope> scope)
    : name(name), imports(imports), decls(std::move(decls)), scope(scope) {};
  
  void pass(ASTVisitor *visitor) override { visitor->visit(this); }

  /// Gets the declarations of this package unit.
  inline std::vector<Decl *> get_decls() const {
    std::vector<Decl *> decls = {};
    for (Decl *d : decls) {
      decls.push_back(d);
    }
    return decls;
  }

  /// Gets the name of this package unit.
  inline const std::string get_name() const { return name; }

  /// Returns the scope of this package unit.
  inline std::shared_ptr<Scope> get_scope() const { return scope; }

  /// Returns the imports of this package unit.
  inline std::vector<std::string> get_imports() const { return imports; }

  /// Returns a string representation of this package unit.
  const std::string to_string() override;
};


/// A crate is a collection of packages and represents a whole program.
class CrateUnit final : public Unit
{
private:
  std::vector<std::unique_ptr<PackageUnit>> packages;

public:
  CrateUnit(std::vector<std::unique_ptr<PackageUnit>> packages) : packages(std::move(packages)) {};

  void pass(ASTVisitor *visitor) override { visitor->visit(this); }

  /// Returns the packages of this crate unit.
  inline const std::string pkg_scope_to_string(const std::string &name) const { 
    for (std::unique_ptr<PackageUnit> const &package : packages) {
      if (package->get_name() == name) {
        return package->get_scope()->to_string();
      }
    }
    return "";
  }

  /// Returns the packages of this crate unit.
  inline std::vector<PackageUnit *> get_packages() const {
    std::vector<PackageUnit *> pkgs = {};
    for (std::unique_ptr<PackageUnit> const &package : packages) {
      pkgs.push_back(package.get());
    }
    return pkgs;
  }

  /// Returns a string representation of this crate unit.
  const std::string to_string() override;
};

#endif  // UNIT_STATIMC_H
