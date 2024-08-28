#ifndef STATIMC_BUILDER_H
#define STATIMC_BUILDER_H

/// Builder for the abstract syntax tree.
/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include "Unit.h"
#include "../core/CContext.h"

/// Builds an abstract syntax tree from the given context.
///
/// This function returns a pointer to the crate root of the tree.
std::unique_ptr<Unit> build_ast(std::unique_ptr<CContext> &Cctx);

#endif  // STATIMC_BUILDER_H
