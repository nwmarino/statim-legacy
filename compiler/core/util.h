/// Copyright 2024 Nick Marino (github.com/nwmarino)

#ifndef STATIMC_UTIL_H
#define STATIMC_UTIL_H

#include <fstream>
#include <string>

#include "ast.h"
#include "cctx.h"
#include "logger.h"

/// Read the contents of a file to a string.
[[nodiscard]]
inline std::string read_to_str(const std::string &path) {
  std::ifstream file(path);

  if (!file.is_open()) {
    panic("could not open file: ", path.c_str());
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

/// Dump all tokens currently in a lexer stream to a file.
inline void dump_tkstream(std::shared_ptr<cctx> ctx) {
  std::ofstream file("tokens.txt");

  file << ctx->filename() << "\n\n";

  while (1) {
    struct Token token = ctx->tk_next();
    if (token.kind == TokenKind::Eof) {
      break;
    }
    file << token.to_str() << '\n';
  }

  file.close();
}

/// Write an ast to a file.
inline void write_ast(std::unique_ptr<ProgAST> ast) {
  std::ofstream file("ast.txt");

  file << ast->to_str();

  file.close();
}

#endif  // STATIMC_UTIL_H
