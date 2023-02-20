#include "catch2/catch_session.hpp"


#include "llvm/Support/TargetSelect.h"

auto main(int argc, char *argv[]) -> int {
  return Catch::Session{}.run(argc, argv);
}