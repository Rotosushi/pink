
#include "Test.h"

#include "llvm/Support/TargetSelect.h"

/*
    Tests don't ensure that the code is correct,
    Tests only ensure that the code is correct in every way you checked.
*/

int main()
{
  llvm::InitializeAllTargetInfos();
  llvm::InitializeAllTargets();
  llvm::InitializeAllTargetMCs();
  llvm::InitializeAllAsmPrinters();
  llvm::InitializeAllAsmParsers();
  llvm::InitializeAllDisassemblers();

  size_t results = RunTests(std::cout, TEST_ALL);

  PrintPassedTests(std::cout, results);

  return 0;
}
