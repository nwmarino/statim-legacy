/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <filesystem>
#include <iostream>
#include <memory>

#include "core/cctx.h"
#include "core/token.h"
#include "core/util.h"
#include "parser/parser.h"

/// Consume and print out all tokens currently in a lexer stream.
static void print_tkstream(std::shared_ptr<cctx> ctx) {
  while (1) {
    struct Token token = ctx->tk_next();
    if (token.kind == TokenKind::Eof) {
      break;
    }
    std::cout << token.to_str() << '\n';
  }
}


/// Parse command line arguments.
static void parse_args(int argc, char *argv[], cflags &flags) {
  flags.emit_asm = false;
  
  for (int i = 1; i < argc; i++) {
    if (std::string(argv[i]) == "-S") {
      flags.emit_asm = true;
    }
  }
}


/// Parse the program source tree.
static std::vector<cfile> parse_files(std::vector<cfile> files, std::filesystem::path dir) {
  for (const std::filesystem::directory_entry &dir_entry : std::filesystem::directory_iterator{dir}) {
    if (dir_entry.is_directory()) {
      files = parse_files(files, dir_entry.path());
    }

    if (dir_entry.is_regular_file()) {
      if (dir_entry.path().extension() == ".statim" || dir_entry.path().extension() == ".stm") {
        cfile file;
        file.filename = parse_filename(dir_entry.path().string());
        file.path = dir_entry.path().string();
        files.push_back(file);
      }
    }
  }

  return files;
}


/// Main entry point for the compiler.
int main(int argc, char *argv[]) {
  cflags flags;
  
  parse_args(argc, argv, flags);
  std::vector<cfile> files = parse_files(std::vector<cfile>(), std::filesystem::current_path());

  std::shared_ptr<cctx> ctx = std::make_shared<cctx>(flags, std::move(files));

  std::unique_ptr<ProgAST> prog = parse_prog(ctx);
  write_ast(std::move(prog));
  //print_tkstream(ctx);
  //dump_tkstream(ctx);
  return 0;
}
