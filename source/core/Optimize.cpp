#include "core/Optimize.h"

namespace pink {
auto Optimize(std::ostream &out, Environment &env) {
  if (env.options->optimization_level != llvm::OptimizationLevel::O0) {
    llvm::LoopAnalysisManager LAM;
    llvm::FunctionAnalysisManager FAM;
    llvm::CGSCCAnalysisManager CGAM;
    llvm::ModuleAnalysisManager MAM;

    // https://llvm.org/doxygen/classllvm_1_1PassBuilder.html
    llvm::PassBuilder passBuilder;

    // #TODO: what are the default Alias Analysis that this constructs?
    FAM.registerPass([&] { return passBuilder.buildDefaultAAPipeline(); });

    // Register the AnalysisManagers with the Pass Builder
    passBuilder.registerModuleAnalyses(MAM);
    passBuilder.registerCGSCCAnalyses(CGAM);
    passBuilder.registerFunctionAnalyses(FAM);
    passBuilder.registerLoopAnalyses(LAM);
    // This registers each of the passes with eachother, so they
    // can perform optimizations together, lazily
    passBuilder.crossRegisterProxies(LAM, FAM, CGAM, MAM);

    llvm::ModulePassManager MPM = passBuilder.buildPerModuleDefaultPipeline(
        env.options->optimization_level);

    // Run the optimizer agains the IR
    MPM.run(*env.llvm_module, MAM);
  }
  return EXIT_SUCCESS;
}
} // namespace pink