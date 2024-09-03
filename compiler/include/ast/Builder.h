#ifndef STATIMC_BUILDER_H
#define STATIMC_BUILDER_H

/// Builder for the abstract syntax tree.
/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <memory>

class ASTContext;
class CrateUnit;

/// Builds an abstract syntax tree from the given context.
///
/// This function returns a pointer to the crate root of the tree.
std::unique_ptr<CrateUnit> build_ast(std::unique_ptr<ASTContext> &Cctx);

#endif  // STATIMC_BUILDER_H
