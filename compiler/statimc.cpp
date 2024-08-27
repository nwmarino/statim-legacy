#include <filesystem>
#include <iostream>

#include "core/CContext.h"
#include "core/Token.h"
#include "core/util.h"

/// Consume and print out all tokens currently in a lexer stream.
static void print_tkstream(std::unique_ptr<CContext> &Cctx) {
  while (1) {
    struct Token token = Cctx->next();
    if (token.kind == TokenKind::Eof) {
      break;
    }
    std::cout << token.to_str() << '\n';
  }
}


/// Parse command line arguments.
static void parse_args(int argc, char *argv[], CFlags &flags) {
  flags.emit_asm = false;
  
  for (int i = 1; i < argc; i++) {
    if (std::string(argv[i]) == "-S") {
      flags.emit_asm = true;
    }
  }
}


/// Parse the program source tree.
static std::vector<CFile> parse_files(std::vector<CFile> files, std::filesystem::path dir) {
  for (const std::filesystem::directory_entry &entry : std::filesystem::directory_iterator{dir}) {
    if (entry.is_directory()) {
      files = parse_files(files, entry.path());
    }

    if (entry.is_regular_file() && entry.path().extension() == ".statim" ) {
      CFile file;
      file.filename = parse_filename(entry.path().string());
      file.path = entry.path().string();
      files.push_back(file);
    }
  }

  return files;
}


/// Main entry point for the compiler.
int main(int argc, char *argv[]) {
  CFlags flags;
  parse_args(argc, argv, flags);
  std::vector<CFile> files = parse_files(std::vector<CFile>(), std::filesystem::current_path());

  if (files.size() == 0) {
    panic("no source files found in cwd: " + std::filesystem::current_path().string());
  }

  std::unique_ptr<CContext> Cctx = std::make_unique<CContext>(flags, std::move(files));

}
