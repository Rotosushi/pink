
#include "Test.h"

#include "llvm/Support/TargetSelect.h"

/*
    Tests don't ensure that the code is correct,
    Tests only ensure that the code is correct in every way you checked.
*/

auto main() -> int {
  // llvm::InitializeAllTargetInfos();
  // llvm::InitializeAllTargets();
  // llvm::InitializeAllTargetMCs();
  // llvm::InitializeAllAsmPrinters();
  // llvm::InitializeAllAsmParsers();
  // llvm::InitializeAllDisassemblers();
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmParser();
  llvm::InitializeNativeTargetDisassembler();

  Testbench bench;
  bench.SetTestsRun(Testbench::all, true);

  bool result = bench.RunTests(std::cout);

  bench.PrintPassedTests(std::cout);

  return static_cast<int>(result);
}
