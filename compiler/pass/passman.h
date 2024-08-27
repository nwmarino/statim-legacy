#ifndef PASSMAN_STATIMC_H
#define PASSMAN_STATIMC_H

/// Pass manager for the AST.
/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include "../core/ast.h"
#include "../core/cctx.h"

void InitializePass(std::shared_ptr<cctx> cctx);

#endif  // PASSMAN_STATIMC_H