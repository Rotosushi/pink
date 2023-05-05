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

#include "core/Compile.h"
#include "core/Link.h"

#include "aux/Environment.h" // pink::CompilationUnit

namespace pink {
auto Compile(int argc, char **argv) -> int {
  auto &out         = std::cout;
  auto &err         = std::cerr;
  auto  cli_options = pink::ParseCLIOptions(err, argc, argv);
  if (!cli_options) {
    cli_options.GetSecond().Print(out);
  }
  auto env = pink::CompilationUnit::CreateNativeCompilationUnit(
      cli_options.GetFirst());

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
