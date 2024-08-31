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
    /// Basic constructor for package units.
    PackageUnit(const std::string &name, std::vector<std::string> imports, std::vector<std::unique_ptr<Decl>> decls, std::shared_ptr<Scope> scope)
      : name(name), imports(imports), decls(std::move(decls)), scope(scope) {};

    /// Gets the name of this package unit.
    [[nodiscard]]
    inline const std::string get_name() const { return name; }

    /// Returns the scope of this package unit.
    [[nodiscard]]
    inline std::shared_ptr<Scope> get_scope() const { return scope; }

    /// Returns a string representation of this package unit.
    [[nodiscard]]
    const std::string to_string();
};


/// A crate is a collection of packages and represents a whole program.
class CrateUnit final : public Unit
{
  private:
    std::vector<std::unique_ptr<PackageUnit>> packages;

  public:
    /// Basic constructor for crate units.
    CrateUnit(std::vector<std::unique_ptr<PackageUnit>> packages) : packages(std::move(packages)) {};

    /// Returns the packages of this crate unit.
    [[nodiscard]]
    inline const std::string pkg_scope_to_string(const std::string &name) const { 
      for (std::unique_ptr<PackageUnit> const &package : packages) {
        if (package->get_name() == name) {
          return package->get_scope()->to_string();
        }
      }
      return "";
    }

    /// Returns a string representation of this crate unit.
    [[nodiscard]]
    const std::string to_string();
};

#endif  // UNIT_STATIMC_H
