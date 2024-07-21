/// Copyright 2024 Nick Marino (github.com/nwmarino)

#ifndef STATIMC_CODEGEN_H
#define STATIMC_CODEGEN_H

#include <memory>

#include "container.h"

/**
 * Initialize the code generation parameters.
 * 
 * @param container The container to unpack.
 */
void initializeModule(std::shared_ptr<LLContainer> container);

#endif  // STATIMC_CODEGEN_H
