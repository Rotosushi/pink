#include <fstream> // std::fstream

#include "core/Compile.h"
#include "core/Link.h"

#include "support/EmitFile.h"

#include "aux/Error.h" // pink::FatalError

#include "llvm/Analysis/AliasAnalysis.h" // llvm::AAManager
#include "llvm/Passes/PassBuilder.h"     // llvm::PassBuilder

namespace pink {
auto Compile(std::ostream &err, Environment &env) -> int;

auto Compile(int argc, char **argv) -> int {
  auto &out = std::cout;
  auto &err = std::cerr;
  auto options = pink::ParseCLIOptions(err, argc, argv);
  auto env = Environment::NewGlobalEnv(options);

  if (Compile(err, *env) == EXIT_FAILURE) {
    return EXIT_FAILURE;
  }

  if (EmitFiles(err, *env) == EXIT_FAILURE) {
    return EXIT_FAILURE;
  }

  return Link(out, err, *env);
}

auto Compile(std::ostream &err, Environment &env) -> int {
  std::vector<std::unique_ptr<pink::Ast>> valid_terms;

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

      auto type_result = term->Typecheck(env);
      if (!type_result) {
        auto &error = type_result.GetSecond();
        env.PrintErrorWithSourceText(err, error);
        return EXIT_FAILURE;
      }

      valid_terms.emplace_back(std::move(term));
    }
    env.ClearFalseBindings();
  }

  // we can very naturally support more than one error
  // here, as we have a buffer of all of the terms already.
  auto failed = false;
  for (std::unique_ptr<pink::Ast> &term : valid_terms) {
    pink::Outcome<llvm::Value *, pink::Error> value = term->Codegen(env);

    if (!value) {
      failed = true;
      auto error = value.GetSecond();
      env.PrintErrorWithSourceText(err, error);
    }
  }
  if (failed) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
} // namespace pink
