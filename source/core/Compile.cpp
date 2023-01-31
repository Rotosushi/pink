#include <fstream> // std::fstream

#include "core/Compile.h"
#include "core/Link.h"

#include "support/EmitFile.h"

#include "aux/Error.h" // pink::FatalError

#include "ast/action/Codegen.h"
#include "ast/action/Typecheck.h"

#include "llvm/Analysis/AliasAnalysis.h" // llvm::AAManager
#include "llvm/Passes/PassBuilder.h"     // llvm::PassBuilder

namespace pink {
auto Compile(std::ostream &err, Environment &env) -> int;

auto Compile(int argc, char **argv) -> int {
  auto &out     = std::cout;
  auto &err     = std::cerr;
  auto  options = pink::ParseCLIOptions(err, argc, argv);
  auto  env     = Environment::NewGlobalEnv(options);

  if (Compile(err, *env) == EXIT_FAILURE) {
    return EXIT_FAILURE;
  }

  if (EmitFiles(err, *env) == EXIT_FAILURE) {
    return EXIT_FAILURE;
  }

  return Link(out, err, *env);
}

auto Compile(std::ostream &err, Environment &env) -> int {
  std::vector<Ast::Pointer> valid_terms;

  { // empty scope, to destroy infile after we are done using it.
    std::fstream infile;
    infile.open(env.options->input_file);
    if (!infile.is_open()) {
      err << "Could not open input file " << env.options->input_file;
      return EXIT_FAILURE;
    }
    env.parser->SetIStream(&infile);

    while (!env.parser->EndOfInput()) {
      auto term_result = env.parser->Parse(env);
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
    env.ClearFalseBindings();
  }

  for (auto &term : valid_terms) {
    auto *value = Codegen(term, env);
  }
  return EXIT_SUCCESS;
}
} // namespace pink
