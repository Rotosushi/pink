
#include "aux/CLIOptions.h"
#include "aux/Environment.h"

#include "core/Compile.h"
#include "core/Link.h"

#include "support/EmitFile.h"

#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/TargetSelect.h"

auto main(int argc, char **argv) -> int {
  llvm::InitLLVM llvm(argc, argv);

  llvm::InitializeAllTargetInfos();
  llvm::InitializeAllTargets();
  llvm::InitializeAllTargetMCs();
  llvm::InitializeAllAsmPrinters();
  llvm::InitializeAllAsmParsers();
  llvm::InitializeAllDisassemblers();
  // llvm::InitializeNativeTarget();
  // llvm::InitializeNativeTargetAsmPrinter();
  // llvm::InitializeNativeTargetAsmParser();
  // llvm::InitializeNativeTargetDisassembler();

  auto options = pink::ParseCLIOptions(std::cout, argc, argv);
  auto env = pink::NewGlobalEnv(options);

  // note: the general strategy for multiple source file compilation
  // is going to be one environment per source file. then we can choose
  // to compile each env in series or parellel. (of course the link step
  // will need to be informed of all object files) note also that we
  // can naturally do this with the definition of the environment, the only
  // hard point is going to be transmitting relevant definitions accross
  // compilation units.
  pink::Compile(*env);

  if (env->options->emit_llvm) {
    EmitLLVMFile(*env, env->options->GetLLVMFilename());
  }

  if (env->options->emit_assembly) {
    EmitAssemblyFile(*env, env->options->GetAsmFilename());
  }

  if (env->options->emit_object) {
    EmitObjectFile(*env, env->options->GetObjFilename());
  }

  // if due to the options we are linking a program,
  // and we have emitted an object file,
  // then we may safely call link.
  if (env->options->link && env->options->emit_object) {
    pink::Link(*env);
  }

  return 0;
}
