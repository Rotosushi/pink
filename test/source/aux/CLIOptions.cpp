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

#include "catch2/catch_test_macros.hpp"

#include "aux/CLIOptions.h"

#include <sstream>

TEST_CASE("aux/CLIFlags", "[unit][aux]") {
  pink::CLIFlags flags;
  // this is overkill
  REQUIRE(flags.DoEmitAssembly() == false);
  REQUIRE(flags.DoEmitAssembly(true) == true);
  REQUIRE(flags.DoEmitAssembly() == true);
  REQUIRE(flags.DoEmitAssembly(false) == false);
  REQUIRE(flags.DoEmitAssembly() == false);

  REQUIRE(flags.DoEmitObject() == true);
  REQUIRE(flags.DoEmitObject(false) == false);
  REQUIRE(flags.DoEmitObject() == false);
  REQUIRE(flags.DoEmitObject(true) == true);
  REQUIRE(flags.DoEmitObject() == true);

  REQUIRE(flags.DoLink() == true);
  REQUIRE(flags.DoLink(false) == false);
  REQUIRE(flags.DoLink() == false);
  REQUIRE(flags.DoLink(true) == true);
  REQUIRE(flags.DoLink() == true);
}

TEST_CASE("aux/CLIOptions", "[unit][aux]") {
  const auto *infile  = "infile.p";
  auto        outfile = pink::CLIOptions::RemoveTrailingExtensions(infile);

  REQUIRE(outfile == "infile");

  pink::CLIOptions options{infile,
                           outfile,
                           pink::CLIFlags{},
                           llvm::OptimizationLevel::O1};

  REQUIRE(options.DoEmitAssembly() == false);
  REQUIRE(options.DoEmitObject() == true);
  REQUIRE(options.DoLink() == true);
  REQUIRE(options.GetOptimizationLevel() == llvm::OptimizationLevel::O1);
  REQUIRE(options.GetInputFile() == infile);
  REQUIRE(options.GetExecutableFile() == outfile);
  REQUIRE(options.GetAssemblyFile() == outfile + ".s");
  REQUIRE(options.GetObjectFile() == outfile + ".ll");

  std::stringstream version;
  pink::CLIOptions::PrintVersion(version);
  REQUIRE(!version.str().empty());

  std::stringstream help;
  pink::CLIOptions::PrintHelp(help);
  REQUIRE(!help.str().empty());
}