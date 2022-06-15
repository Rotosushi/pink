
#include "aux/CLIOptions.h"
#include "aux/Environment.h"

#include "core/Compile.h"


int main(int argc, char** argv)
{
	pink::CLIOptions options = pink::ParseCLIOptions(std::cout, argc, argv);
	
  llvm::InitializeAllTargetInfos();
  llvm::InitializeAllTargets();
  llvm::InitializeAllTargetMCs();
  llvm::InitializeAllAsmPrinters();
  llvm::InitializeAllAsmParsers();
  llvm::InitializeAllDisassemblers();

  std::shared_ptr<pink::Environment> env = pink::NewGlobalEnv(options);
               
    // Call the main driver code         
	Compile(options, env, target_machine);

  return 0;
}
