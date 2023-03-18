// Copyright (C) 2023 cadence
// 
// This file is part of pink.
// 
// pink is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// pink is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with pink.  If not, see <http://www.gnu.org/licenses/>.

#include <fstream> // std::fstream

#include "core/Compile.h"
#include "core/Link.h"

#include "aux/Error.h" // pink::FatalError

#include "ast/action/Codegen.h"
#include "ast/action/Typecheck.h"

#include "llvm/Analysis/AliasAnalysis.h" // llvm::AAManager

#include "llvm/Passes/PassBuilder.h" // llvm::PassBuilder

#include "llvm/Support/TargetSelect.h"

namespace pink {
static auto CompileEnvironment(std::ostream &out,
                               std::ostream &err,
                               Environment  &env) -> int;

auto Compile(int argc, char **argv) -> int {
  auto &out         = std::cout;
  auto &err         = std::cerr;
  auto  cli_options = pink::ParseCLIOptions(err, argc, argv);
  auto  env         = pink::Environment::CreateNativeEnvironment(cli_options);

  if (CompileEnvironment(out, err, env) == EXIT_FAILURE) {
    return EXIT_FAILURE;
  }

  if (env.DefaultAnalysis(err) == EXIT_FAILURE) {
    return EXIT_FAILURE;
  }

  if (env.EmitFiles(err) == EXIT_FAILURE) {
    return EXIT_FAILURE;
  }

  if (!env.DoLink()) {
    return EXIT_SUCCESS;
  }

  return Link(out, err, env);
}

using Terms = std::vector<pink::Ast::Pointer>;

static auto ParseAndTypecheckInputFile(std::ostream &err, Environment &env)
    -> std::pair<Terms, bool> {
  Terms valid_terms;
  Terms invalid_terms;

  { // empty scope, to destroy infile after we are done using it.
    std::fstream infile;
    infile.open(env.GetInputFilename().data());
    if (!infile.is_open()) {
      err << "Could not open input file [" << env.GetInputFilename() << "]\n";
      return {};
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
        return {};
      }
      auto &term = term_result.GetFirst();

      auto type_result = Typecheck(term, env);
      if (!type_result) {
        auto &error = type_result.GetSecond();
        env.PrintErrorWithSourceText(err, error);
        invalid_terms.emplace_back(std::move(term));
      } else {
        valid_terms.emplace_back(std::move(term));
      }
    }

    // reset the scopes so that Codegen
    // doesn't redeclare variables when
    // it processes this env.
    // #NOTE: 2/23/23
    // This is a bit weird, and I am thinking
    // about a better way to handle this
    // issue of symbol redefinition between
    // Typechecking and Codegeneration.
    env.ResetScopes();
  }
  if (invalid_terms.empty()) {
    return {std::move(valid_terms), true};
  }
  return {std::move(invalid_terms), false};
}

static void CodegenValidTerms(const Terms &valid_terms, Environment &env) {
  for (const auto &term : valid_terms) {
    auto *value = Codegen(term, env);
    assert(value != nullptr);
  }
}

auto CompileEnvironment(std::ostream &out, std::ostream &err, Environment &env)
    -> int {
  if (env.DoVerbose()) {
    out << "Compiling source file [" << env.GetInputFilename() << "]\n";
  }

  auto terms = ParseAndTypecheckInputFile(err, env);
  if (!terms.second) {
    // we can do something with invalid terms here if we wanted/needed to
    return EXIT_FAILURE;
  }
  auto &valid_terms = terms.first;

  CodegenValidTerms(valid_terms, env);

  return EXIT_SUCCESS;
}
} // namespace pink
