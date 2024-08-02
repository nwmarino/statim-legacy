// Copyright 2024 Nick Marino (github.com/nwmarino)

#include <cstdio>
#include <fstream>
#include <map>
#include <memory>
#include <utility>

#include "cgn.h"
#include "../core/logger.h"
#include "register.h"

static std::map<const char *, std::unique_ptr<Register>> reg_table;
static std::ofstream out_file;

static void init_all_registers()
{
  reg_table["r8"] = std::make_unique<Register>("r8");
  reg_table["r9"] = std::make_unique<Register>("r9");
  reg_table["r10"] = std::make_unique<Register>("r10");
  reg_table["r11"] = std::make_unique<Register>("r11");
  reg_table["eax"] = std::make_unique<Register>("eax");
}

static void free_register(const char *id)
{
  if (reg_table[id]) {
    reg_table[id]->free();
    return;
  }
  panic("Error: Attempted to free unknown register.", id);
}

static void free_all_registers()
{
  for (auto &reg : reg_table) {
    reg.second->free();
  }
}

static const char *alloc_register()
{
  for (auto &reg : reg_table) {
    if (reg.second->is_free()) {
      reg.second->alloc();
      return reg.first;
    }
  }
  panic("Error: No free registers available.", nullptr);
  return nullptr;
}

static const char *cg_load_int(int val, std::optional<const char *> __reg)
{
  const char *reg_id;
  if (__reg)
    reg_id = __reg.value();
  else
    reg_id = alloc_register();

  out_file << "\tmovl\t$" << val << ", %" << reg_id << std::endl;
  return reg_id;
}

/**
 * Initialize the code generator.
 * 
 * @param out_file_name The name of the output file.
 */
void cgn_init(const char *out_file_name)
{
  init_all_registers();
  out_file.open(out_file_name);
}

void cgn_close()
{
  out_file.close();
}

void FunctionAST::codegen()
{
  head->codegen();
  body->codegen();
}

void PrototypeAST::codegen()
{
  out_file << "\t.text\n"
           << "\t.globl\t" << getName() << "\n"
           << "\t.type\t" << getName() << ", @function\n"
           << getName() << ":\n"
           << "\tpushq\t%rbp\n"
           << "\tmovq\t%rsp, %rbp\n";
}

void CompoundStatement::codegen()
{
  for (std::unique_ptr<Statement> &stmt : stmts) {
    stmt->codegen();
  }
}

void ReturnStatement::codegen()
{
  expr->codegen();

  out_file << "\tpopq\t%rbp" << std::endl
           << "\tret" << std::endl;
}

void IntegerExpr::codegen()
{
  const char *reg = cg_load_int(value, "eax");
  free_register(reg);
}

void FunctionCallExpr::codegen()
{

}

void FloatingPointExpr::codegen()
{

}

void BinaryExpr::codegen()
{

}

void VariableExpr::codegen()
{

}
