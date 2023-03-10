#include "core/Compile.h"

#include "llvm/Support/InitLLVM.h"

auto main(int argc, char **argv) -> int {
  llvm::InitLLVM llvm{argc, argv};

  // llvm::InitializeAllTargetInfos();
  // llvm::InitializeAllTargets();
  // llvm::InitializeAllTargetMCs();
  // llvm::InitializeAllAsmPrinters();
  // llvm::InitializeAllAsmParsers();
  // llvm::InitializeAllDisassemblers();
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetAsmParser();
  llvm::InitializeNativeTargetDisassembler();

  return pink::Compile(argc, argv);
}
