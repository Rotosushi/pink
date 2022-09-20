
#include "aux/CLIOptions.h"
#include "aux/Environment.h"

#include "core/Compile.h"
#include "core/Link.h"

#include "support/EmitFile.h"

#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/TargetSelect.h"

int main(int argc, char** argv)
{
  llvm::InitLLVM llvm(argc, argv);

  llvm::InitializeAllTargetInfos();
  llvm::InitializeAllTargets();
  llvm::InitializeAllTargetMCs();
  llvm::InitializeAllAsmPrinters();
  llvm::InitializeAllAsmParsers();
  llvm::InitializeAllDisassemblers();
  //llvm::InitializeNativeTarget();
  //llvm::InitializeNativeTargetAsmPrinter();
  //llvm::InitializeNativeTargetAsmParser();
  //llvm::InitializeNativeTargetDisassembler();


  std::shared_ptr<pink::CLIOptions> options = pink::ParseCLIOptions(std::cout, argc, argv);
  std::unique_ptr<pink::Environment> env = pink::NewGlobalEnv(options);
  
  // compile the given source file
  pink::Compile(*env);

  // emit the requested output format
  if (env->options->emit_llvm)
    EmitLLVMFile(*env, env->options->GetLLVMFilename());

  if (env->options->emit_assembly)
    EmitAssemblyFile(*env, env->options->GetAsmFilename());

  if (env->options->emit_object)
    EmitObjectFile(*env, env->options->GetObjFilename());

  // if due to the options we are linking a program,
  // and we have emitted an object file, 
  // then we may safely call link.
  if (env->options->link && env->options->emit_object)
    pink::Link(*env);

  return 0;
}
