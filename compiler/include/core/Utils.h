#ifndef STATIMC_UTIL_H
#define STATIMC_UTIL_H

/// Utility functions for the compiler.
/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <filesystem>
#include <fstream>

/// Read the contents of a file to a string.
[[nodiscard]]
inline std::string read_to_str(const std::string &path) {
  std::ifstream file(path);

  if (!file.is_open()) {
    fprintf(stderr, "could not open file: %s\n", path.c_str());
  }

  std::string contents;
  std::string line;
  while (getline(file, line)) {
    contents.append(line + '\n');
  }

  return contents;
}


/// Parse a file name from a path.
[[nodiscard]]
inline std::string parse_filename(const std::string &path) {
  std::size_t pos = path.find_last_of("/\\");
  return path.substr(pos + 1);
}


/// Remove the file extension from a file name.
[[nodiscard]]
inline std::string remove_extension(const std::string &filename) {
  std::size_t pos = filename.find_last_of(".");
  return filename.substr(0, pos);
}


/// Read in the current working directory.
[[nodiscard]]
inline std::string read_cwd(void) {
  return std::filesystem::current_path().string();
}

#endif  // STATIMC_UTIL_H
