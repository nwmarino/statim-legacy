#ifndef UNIT_STATIMC_H
#define UNIT_STATIMC_H

/// Translation unit related AST nodes.
/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <memory>
#include <vector>

#include "Decl.h"

/// Base class for source units.
class Unit
{
  public:
    virtual ~Unit() = default;
    virtual const std::string to_string(int n) = 0;
};


/// A package is the representation of a modular source file.
class PackageUnit : public Unit
{
  private:
    const std::string name;
    std::vector<std::string> imports;
    std::vector<std::unique_ptr<Decl>> decls;
    std::unique_ptr<Scope> scope;

  public:
    /// Basic constructor for package units.
    PackageUnit(const std::string &name, std::vector<std::string> imports, std::vector<std::unique_ptr<Decl>> decls, std::unique_ptr<Scope> scope)
      : name(name), imports(imports), decls(std::move(decls)), scope(std::move(scope)) {};

    /// Gets the name of this package unit.
    [[nodiscard]]
    inline const std::string get_name() const { return name; }

    /// Get the scope object of this package unit.
    [[nodiscard]]
    inline std::unique_ptr<Scope> &get_scope() { return scope; }

    /// Returns a string representation of this package unit.
    [[nodiscard]]
    const std::string to_string(int n);
};


/// A crate is a collection of packages and represents a whole program.
class CrateUnit : public Unit
{
  private:
    std::vector<std::unique_ptr<PackageUnit>> packages;

  public:
    /// Basic constructor for crate units.
    CrateUnit(std::vector<std::unique_ptr<PackageUnit>> packages)
      : packages(std::move(packages)) {};

    /// Returns a string representation of this crate unit.
    [[nodiscard]]
    const std::string to_string(int n);
};

#endif  // UNIT_STATIMC_H
