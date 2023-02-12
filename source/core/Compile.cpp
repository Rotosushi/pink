#include <fstream> // std::fstream

#include "core/Compile.h"

#include "aux/Error.h" // pink::FatalError

#include "ast/action/Codegen.h"
#include "ast/action/Typecheck.h"

#include "llvm/Analysis/AliasAnalysis.h" // llvm::AAManager
#include "llvm/Passes/PassBuilder.h"     // llvm::PassBuilder

namespace pink {
auto Compile(std::ostream &err, Environment &env) -> int {
  std::vector<Ast::Pointer> valid_terms;

  { // empty scope, to destroy infile after we are done using it.
    std::fstream infile;
    infile.open(env.GetInputFilename().data());
    if (!infile.is_open()) {
      err << "Could not open input file " << env.GetInputFilename();
      return EXIT_FAILURE;
    }
    env.SetIStream(&infile);

    while (!env.EndOfInput()) {
      auto term_result = env.Parse();
      if (!term_result) {
        auto &error = term_result.GetSecond();
        if ((error.code == Error::Code::EndOfFile) && (!valid_terms.empty())) {
          break; // out of while loop
        }
        env.PrintErrorWithSourceText(err, error);
        return EXIT_FAILURE;
      }
      auto &term = term_result.GetFirst();

      auto type_result = Typecheck(term, env);
      if (!type_result) {
        auto &error = type_result.GetSecond();
        env.PrintErrorWithSourceText(err, error);
        return EXIT_FAILURE;
      }

      valid_terms.emplace_back(std::move(term));
    }

    env.PopScope();
    env.PushScope();
  }

  for (auto &term : valid_terms) {
    auto *value = Codegen(term, env);
    assert(value != nullptr);
  }
  return EXIT_SUCCESS;
}
} // namespace pink
