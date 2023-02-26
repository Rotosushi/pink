#include "catch2/catch_session.hpp"

#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/TargetSelect.h"

auto main(int argc, char *argv[]) -> int {
  // destructor of InitLLVM destroys llvm ManagedStatic objects
  llvm::InitLLVM llvm(argc, argv);
  // initialize necessary ManagedStatic objects
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetAsmParser();
  llvm::InitializeNativeTargetDisassembler();

  return Catch::Session{}.run(argc, argv);
}