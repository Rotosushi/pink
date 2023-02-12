#include "core/Optimize.h"

#include "llvm/Analysis/AliasAnalysis.h"

namespace pink {
auto DefaultAnalysis(std::ostream &err, Environment &env) -> int {
  assert(err.good());
  llvm::LoopAnalysisManager     LAM;
  llvm::FunctionAnalysisManager FAM;
  llvm::CGSCCAnalysisManager    CGAM;
  llvm::ModuleAnalysisManager   MAM;

  // https://llvm.org/doxygen/classllvm_1_1PassBuilder.html
  llvm::PassBuilder passBuilder;

  // #TODO: what are the default analysis that this constructs?
  FAM.registerPass([&] { return passBuilder.buildDefaultAAPipeline(); });

  // Register the AnalysisManagers with the Pass Builder
  passBuilder.registerModuleAnalyses(MAM);
  passBuilder.registerCGSCCAnalyses(CGAM);
  passBuilder.registerFunctionAnalyses(FAM);
  passBuilder.registerLoopAnalyses(LAM);
  // This registers each of the passes with eachother, so they
  // can perform optimizations together, lazily
  passBuilder.crossRegisterProxies(LAM, FAM, CGAM, MAM);

  llvm::ModulePassManager MPM =
      passBuilder.buildPerModuleDefaultPipeline(env.GetOptimizationLevel());

  // Run the optimizer against the IR
  MPM.run(*env.module, MAM);
  return EXIT_SUCCESS;
}
} // namespace pink