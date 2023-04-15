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
auto Compile(int argc, char **argv) -> int {
  auto &out         = std::cout;
  auto &err         = std::cerr;
  auto  cli_options = pink::ParseCLIOptions(err, argc, argv);
  auto  env = pink::CompilationUnit::CreateNativeEnvironment(cli_options);

  if (env.Compile(out, err) == EXIT_FAILURE) {
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
} // namespace pink
