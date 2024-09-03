#include <filesystem>
#include <iostream>

#include "include/ast/Builder.h"
#include "include/token/Token.h"
#include "include/core/ASTContext.h"
#include "include/ast/Unit.h"
#include "include/core/Utils.h"
#include "include/core/Logger.h"

/// Consume and print out all tokens currently in a lexer stream.
static void print_tkstream(std::unique_ptr<ASTContext> &Cctx) {
  do {
    Cctx->next_file();
    while (!Cctx->last().is_eof()) {
      Cctx->next();
      std::cout << Cctx->last().to_str() + '\n';
    }
  } while (!Cctx->last().is_eof());
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

  std::unique_ptr<ASTContext> ctx = std::make_unique<ASTContext>(flags, std::move(files));
  std::unique_ptr<CrateUnit> crate = build_ast(ctx);
  std::cout << crate->to_string();
}
