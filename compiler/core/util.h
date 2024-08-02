/// Copyright 2024 Nick Marino (github.com/nwmarino)

#ifndef STATIMC_UTIL_H
#define STATIMC_UTIL_H

#include <fstream>
#include <string>

#include "logger.h"

/// @brief  Read the contents of a file to a string.
[[nodiscard]]
inline std::string read_to_str(const std::string &path) {
  std::ifstream file(path);

  if (!file.is_open()) {
    panic("could not open file.", path.c_str());
  }

  std::string contents;
  std::string line;
  while (getline(file, line)) {
    contents.append(line);
  }

  return contents;
}

#endif  // STATIMC_UTIL_H
