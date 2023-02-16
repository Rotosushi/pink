#include "catch2/catch_session.hpp"

#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/TargetSelect.h"

auto main(int argc, char *argv[]) -> int {
  llvm::InitLLVM llvm{argc, argv};

  return Catch::Session{}.run(argc, argv);
}