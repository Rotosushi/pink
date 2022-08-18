
#include "aux/CLIOptions.h"
#include "aux/Environment.h"

#include "kernel/Support.h"

#include "core/Compile.h"
// #TODO: add a separate 'emit' step by factoring out the 
// file writing code from Compile()
#include "core/Link.h"

//#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/TargetSelect.h"

int main(int argc, char** argv)
{
  // for some reason calling InitLLVM, in an attempt 
  // to get llvm to clean up more of what it allocates, 
  // causes a segmentation fault within AddPassesToEmitFile.
  // llvm::InitLLVM(argc, argv);

  llvm::InitializeAllTargetInfos();
  llvm::InitializeAllTargets();
  llvm::InitializeAllTargetMCs();
  llvm::InitializeAllAsmPrinters();
  llvm::InitializeAllAsmParsers();
  llvm::InitializeAllDisassemblers();

  std::shared_ptr<pink::CLIOptions> options = pink::ParseCLIOptions(std::cout, argc, argv);
	
  std::shared_ptr<pink::Environment> env = pink::NewGlobalEnv(options);
                        
  pink::Compile(env);

  if (env->options->emit_object)
    pink::Link(env);

  return 0;
}
