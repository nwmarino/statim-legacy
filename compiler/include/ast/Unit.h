#ifndef UNIT_STATIMC_H
#define UNIT_STATIMC_H

/// Translation unit related AST nodes.
/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include "Decl.h"

/// Base class for source units.
class Unit
{
  public:
    virtual ~Unit() = default;
    const virtual std::string to_string(int n) = 0;
};


/// A package is the representation of a modular source file.
class PackageUnit : public Unit
{
  private:
    const std::string name;
    std::vector<std::unique_ptr<Decl>> decls;

  public:
    /// Basic constructor for package units.
    PackageUnit(const std::string &name, std::vector<std::unique_ptr<Decl>> decls)
      : name(name), decls(std::move(decls)) {};

    /// Gets the name of this package unit.
    [[nodiscard]]
    inline const std::string get_name() const { return name; }

    /// Gets the declarations of this package unit.
    [[nodiscard]]
    inline const std::vector<std::unique_ptr<Decl>> get_decls() const { return decls; }

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

    /// Gets the packages of this crate unit.
    [[nodiscard]]
    inline const std::vector<std::unique_ptr<PackageUnit>> get_packages() const { return packages; }

    /// Returns a string representation of this crate unit.
    [[nodiscard]]
    const std::string to_string(int n);
};

#endif  // UNIT_STATIMC_H
