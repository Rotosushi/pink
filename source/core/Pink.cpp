#include "core/Compile.h"
#include "core/Link.h"
#include "core/Optimize.h"

#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/TargetSelect.h"

auto main(int argc, char **argv) -> int {
  llvm::InitLLVM llvm{argc, argv};

  // llvm::InitializeAllTargetInfos();
  // llvm::InitializeAllTargets();
  // llvm::InitializeAllTargetMCs();
  // llvm::InitializeAllAsmPrinters();
  // llvm::InitializeAllAsmParsers();
  // llvm::InitializeAllDisassemblers();
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetAsmParser();
  llvm::InitializeNativeTargetDisassembler();

  auto &out         = std::cout;
  auto &err         = std::cerr;
  auto  cli_options = pink::ParseCLIOptions(err, argc, argv);
  auto  env         = pink::Environment::CreateNativeEnvironment(cli_options);

  if (Compile(err, env) == EXIT_FAILURE) {
    return EXIT_FAILURE;
  }

  // always run at least the default analysis [-O0]
  // to clean up emitted IR before link time optimization.
  // to speed up the linker's job by getting rid of
  // low hanging fruit first.
  pink::DefaultAnalysis(err, env);

  if (env.EmitFiles(err) == EXIT_FAILURE) {
    return EXIT_FAILURE;
  }

  if (env.DoLink()) {
    return Link(out, err, env);
  }

  return EXIT_SUCCESS;
}
