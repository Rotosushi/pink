

#include "core/Compile.h"

#include <fstream> // std::fstream

#include "aux/Error.h" // pink::FatalError

#include "llvm/Analysis/AliasAnalysis.h" // llvm::AAManager
#include "llvm/Passes/PassBuilder.h"     // llvm::PassBuilder

namespace pink {
void Compile(const Environment &env) {
  std::fstream infile;

  infile.open(env.options->input_file);

  if (!infile.is_open()) {
    FatalError("Could not open input file " + env.options->input_file, __FILE__,
               __LINE__);
  }

  // it's wasteful to assign the parser when
  // we already constructed it, but due to the reference
  // that the parser holds to a std::istream we cannot
  // modify what the parser is reading from after construction.
  // so we must reconstruct here to only have the output file open
  // during compilation.
  env.parser->SetIStream(&infile);

  std::vector<std::unique_ptr<pink::Ast>> valid_terms;

  while (!env.parser->EndOfInput()) {
    auto term = env.parser->Parse(env);

    if (!term) {
      pink::Error error(term.GetSecond());

      if (error.code == Error::Code::EndOfFile) {
        // if we didn't parse any terms, then there is nothing to optimize,
        // emit, or link together thus we are safe to simply exit.
        if (valid_terms.empty()) {
          FatalError("Parsed an empty source file " + env.options->input_file,
                     __FILE__, __LINE__);
        }
        // else we did parse some terms and simply reached the end of the
        // source file, so we can safely continue with compilation of the
        // valid terms.
        break; // out of while loop
      }
      // #TODO: Handle more than the first error detected in the input code.
      // #TODO: Extract the line that produced the error from the buffer and
      // 			pass it as an argument here.
      std::string bad_source = env.parser->ExtractLine(error.loc);
      FatalError(error.ToString(bad_source), __FILE__, __LINE__);
    } else {
      pink::Outcome<pink::Type *, pink::Error> type =
          term.GetFirst()->Getype(env);

      // if not type and error == use-before-definition
      // {
      //   push term into a llvm::DenseMap<pink::InternedString,
      //   std::pair<std::unique_ptr<pink::Ast>,
      //   std::vector<std::unique_ptr<pink::Ast>>::iterator>>
      //   // where the InternedString holds the variable name which was used
      //   // before it was defined, and std::unique_ptr<pink::Ast> holds the
      //   // term which failed to type. and the std::vector<...>::iterator
      //   // holds the place where we would have inserted this term into the
      //   // original vector. that place where we would have inserted is
      //   // instead filled with a dummy term later, which is intended to be
      //   // replaced by the real term once there are no use-before-definition
      //   // errors.
      //   //
      // }
      if (!type) {
        pink::Error error(type.GetSecond());
        std::string bad_source = env.parser->ExtractLine(error.loc);
        FatalError(error.ToString(bad_source), __FILE__, __LINE__);
      } else {
        valid_terms.push_back(std::move(term.GetFirst()));
      }
    }
  }

  // if there are no invalid terms when we get here.
  // that is, if every bit of source we parsed typechecks
  // then we simply have to emit all of their definitions into
  // the module. (perhaps this too will require a
  // use-before-definition buffer?)
  // however, we have one thing to take care of, after typing all
  // expressions, the Bind form will have constructed false bindings
  // to allow the type of bound names to be used in later expressions.
  // so, we have to unbind each name that was falsely bound,
  // so that Codegen can bind those names to their actual types.
  for (InternedString fbnd : *env.false_bindings) {
    env.bindings->Unbind(fbnd);
  }
  // then since we unbound them, clear the names from the list
  env.false_bindings->clear();

  for (std::unique_ptr<pink::Ast> &term : valid_terms) {
    // as a side effect Codegen builds all of the llvm assembly within the
    // module using the llvm::IRBuilder<>
    pink::Outcome<llvm::Value *, pink::Error> value = term->Codegen(env);

    if (!value) {
      pink::Error error(value.GetSecond());
      std::string bad_source(env.parser->ExtractLine(error.loc));
      FatalError(error.ToString(bad_source), __FILE__, __LINE__);
    }
  }

  // #NOTE:
  //    at this point in execution we are done compiling the source
  //    down to llvm IR.
  infile.close();

  // if the optimization level is greater than zero, optimize the code.
  // #TODO: look more into what would be good optimizations to run.
  if (env.options->optimization_level != llvm::OptimizationLevel::O0) {
    // These are the analysis pass managers that run the actual
    // analysis and optimization code against the IR.
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
    MPM.run(*env.module, MAM);
  }
}
} // namespace pink
