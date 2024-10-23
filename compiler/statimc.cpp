#include <filesystem>
#include <iostream>

#include "llvm/CodeGen/CommandFlags.h"
#include "llvm/IR/IRPrintingPasses.h"
#include "llvm/IR/LegacyPassManager.h"
#include <llvm/Analysis/AliasAnalysis.h>
#include <llvm/IR/PassManager.h>
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include <llvm/IR/Verifier.h>
#include <llvm/Support/CodeGen.h>
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/TargetParser/Host.h"


#include "include/ast/Builder.h"
#include "include/ast/Unit.h"
#include "include/cgn/codegen.h"
#include "include/core/ASTContext.h"
#include "include/core/ASTVisitor.h"
#include "include/core/Utils.h"
#include "include/core/Logger.h"
#include "include/token/Token.h"

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
    } else if (std::string(argv[i]) == "-P1") {
      flags.pass_one = true;
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


llvm::TargetMachine *create_tm() {
  llvm::Triple triple = llvm::Triple(llvm::sys::getDefaultTargetTriple());

  llvm::TargetOptions opts;
  std::string cpu_str = "generic";
  std::string feat_str = "";

  std::string err;
  const llvm::Target *target = llvm::TargetRegistry::lookupTarget(llvm::sys::getDefaultTargetTriple(), err);

  if (!target) {
    panic(err);
  }

  llvm::TargetMachine *tm = target->createTargetMachine(triple.getTriple(), 
    cpu_str, feat_str, opts, llvm::Reloc::PIC_);
  return tm;
}


int write_output(CFlags flags, const std::string &pkg, llvm::TargetMachine *tm, llvm::Module *mod) {
  llvm::PassBuilder pb(tm);

  llvm::LoopAnalysisManager lam;
  llvm::FunctionAnalysisManager fam;
  llvm::CGSCCAnalysisManager cgam;
  llvm::ModuleAnalysisManager mam;

  fam.registerPass([&] { return pb.buildDefaultAAPipeline(); });

  pb.registerModuleAnalyses(mam);
  pb.registerCGSCCAnalyses(cgam);
  pb.registerFunctionAnalyses(fam);
  pb.registerLoopAnalyses(lam);
  pb.crossRegisterProxies(lam, fam, cgam, mam);

  llvm::ModulePassManager mpm;

  llvm::CodeGenFileType file_type = flags.emit_asm ? llvm::CodeGenFileType::AssemblyFile : \
    llvm::CodeGenFileType::ObjectFile;
  std::string output;
  if (flags.emit_asm) {
    output = pkg + ".s";
  } else {
    output = pkg + ".o";
  }

  std::error_code EC;
  llvm::sys::fs::OpenFlags open_flags = llvm::sys::fs::OF_None;
  if (file_type == llvm::CodeGenFileType::AssemblyFile) {
    open_flags |= llvm::sys::fs::OF_Text;
  }
  auto out = std::make_unique<llvm::ToolOutputFile>(output, EC, open_flags);
  if (EC) {
    panic(EC.message());
  }

  llvm::legacy::PassManager cpm;
  if (file_type == llvm::CodeGenFileType::AssemblyFile && flags.emit_llvm_ir) {
    cpm.add(llvm::createPrintModulePass(out->os()));
  } else {
    if (tm->addPassesToEmitFile(cpm, out->os(), nullptr, file_type)) {
      panic("no support for file type");
    }
  }

  //mod->print(llvm::errs(), nullptr);

  if (llvm::verifyModule(*mod, &llvm::errs())) {
    panic("bad codegen");
  }

  mpm.run(*mod, mam);
  cpm.run(*mod);
  out->keep();

  return 1;
}


/// Main entry point for the compiler.
int main(int argc, char *argv[]) {
  llvm::InitializeAllTargets();
  llvm::InitializeAllTargetMCs();
  llvm::InitializeAllTargetInfos();
  llvm::InitializeAllAsmPrinters();
  llvm::InitializeAllAsmParsers();

  CFlags flags;
  parse_args(argc, argv, flags);
  std::vector<CFile> files = parse_files(std::vector<CFile>(), std::filesystem::current_path());

  if (files.size() == 0) {
    panic("no source files found in cwd: " + std::filesystem::current_path().string());
  }

  std::unique_ptr<ASTContext> ctx = std::make_unique<ASTContext>(flags, std::move(files));
  std::unique_ptr<CrateUnit> crate = build_ast(ctx);
  crate->pass(new PassVisitor());

  llvm::TargetMachine *tm = create_tm();
  for (PackageUnit * &pkg : crate->get_packages()) {
    cgn::Codegen cgn(pkg->get_name(), tm);
    pkg->pass(&cgn);

    write_output(flags, pkg->get_name(), tm, cgn.get_module());
  }

  //std::cout << crate->to_string();
}
