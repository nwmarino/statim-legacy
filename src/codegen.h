// Copyright (c) 2024 Nick Marino (github.com/nwmarino)

#ifndef CODEGEN_H
#define CODEGEN_H

#include <memory>

#include "container.h"

void initializeModule(std::shared_ptr<LLContainer> container);
void modulePrint();

#endif  // CODEGEN_H