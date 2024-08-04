/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <iostream>
#include <memory>

#include "core/cctx.h"
#include "core/token.h"
#include "core/util.h"

/// Consume and print out all tokens currently in a lexer stream.
static void print_tkstream(std::shared_ptr<cctx> ctx) {
  while (1) {
    std::unique_ptr<Token> token = ctx->tk_next();
    if (token->kind == TokenKind::Eof) {
      break;
    }
    std::cout << token->to_str() << '\n';
  }
}

/// Parse input files and options from the command line.
static void parse_args(int argc, char *argv[], cflags &flags, std::vector<cfile> &input) {
  flags.emit_asm = false;
  
  for (int i = 1; i < argc; i++) {
    if (std::string(argv[i]) == "-S") {
      flags.emit_asm = true;
    } else {
      cfile file;
      file.filename = parse_filename(argv[i]);
      file.path = argv[i];
      
      input.push_back(file);
    }
  }
}

/// Main entry point for the compiler.
int main(int argc, char *argv[]) {
  if (argc < 2) {
    panic("no input files", nullptr);
  }

  cflags flags;
  std::vector<cfile> input;
  
  parse_args(argc, argv, flags, input);

  std::shared_ptr<cctx> ctx = std::make_shared<cctx>(flags, input);

  //print_tkstream(ctx);
  dump_tkstream(ctx);
  return 0;
}
