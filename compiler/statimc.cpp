/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <iostream>
#include <memory>

#include "core/cctx.h"
#include "core/token.h"

static void print_tkstream(std::shared_ptr<cctx> ctx) {
  while (1) {
    std::unique_ptr<Token> token = ctx->tk_next();
    if (token->kind == TokenKind::Eof) {
      break;
    }
    std::cout << token->to_str() << '\n';
  }
}

static void dump_tkstream(std::shared_ptr<cctx> ctx) {
  while (1) {
    std::unique_ptr<Token> token = ctx->tk_next();
    if (token->kind == TokenKind::Eof) {
      break;
    }
  }
}

int main(int argc, char *argv[]) {
  cflags flags;
  flags.emit_asm = false;

  cfile in;
  in.filename = "ret_zero.statim";
  in.path = "../samples/integer/ret/ret_zero.statim";

  std::shared_ptr<cctx> ctx = std::make_shared<cctx>(flags, std::vector<cfile>(1, in));

  //print_tkstream(ctx);
  dump_tkstream(ctx);
  return 0;
}
