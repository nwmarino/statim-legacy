/// Copyright 2024 Nick Marino (github.com/nwmarino)

#include "llvm/Bitcode/BitcodeWriter.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/TargetParser/Host.h"
#include "llvm/Transforms/Utils/Cloning.h"

#include <string>

#include "write_obj.h"

std::string write_object_file(std::shared_ptr<llvm::Module> LLModule)
{
  llvm::InitializeAllTargetInfos();
  llvm::InitializeAllTargets();
  llvm::InitializeAllTargetMCs();
  llvm::InitializeAllAsmParsers();
  llvm::InitializeAllAsmPrinters();
  
  std::string TargetTriple = llvm::sys::getDefaultTargetTriple();

  std::string error;
  const llvm::Target *Target = llvm::TargetRegistry::lookupTarget(
    TargetTriple, error);

  if (!Target) {
    llvm::errs() << error;
    return "";
  }

  std::string CPU = "generic";
  std::string Features = "";

  llvm::TargetOptions opt;
  llvm::TargetMachine *TM = Target->createTargetMachine(
      TargetTriple, CPU, Features, opt, llvm::Reloc::PIC_
  );

  LLModule->setDataLayout(TM->createDataLayout());
  LLModule->setTargetTriple(TargetTriple);

  const char *Filename = "output.o";
  std::error_code EC;
  llvm::raw_fd_ostream dest(Filename, EC, llvm::sys::fs::OF_None);

  if (EC) {
    llvm::errs() << "Could not open file: " << EC.message();
    return "";
  }

  llvm::legacy::PassManager pass;
  llvm::CodeGenFileType FileType = llvm::CodeGenFileType::ObjectFile;

  if (TM->addPassesToEmitFile(pass, dest, nullptr, FileType)) {
    llvm::errs() << "TheTargetMachine can't emit a file of this type";
    return "";
  }

  pass.run(*LLModule);
  dest.flush();

  return Filename;
}
