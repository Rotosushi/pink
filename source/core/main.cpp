#include "core/Compile.h"

#include "llvm/Support/InitLLVM.h"

auto main(int argc, char **argv) -> int {
  llvm::InitLLVM llvm{argc, argv};
  return pink::Compile(argc, argv);
}
