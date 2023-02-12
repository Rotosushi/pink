#include "catch2/catch_test_macros.hpp"

#include "aux/CLIOptions.h"

#include <sstream>

TEST_CASE("aux/CLIFlags", "[unit][aux]") {
  pink::CLIFlags flags;

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
  REQUIRE(options.GetInputFilename() == infile);
  REQUIRE(options.GetExecutableFilename() == outfile);
  REQUIRE(options.GetAssemblyFilename() == outfile + ".s");
  REQUIRE(options.GetObjectFilename() == outfile + ".ll");

  std::stringstream version;
  pink::CLIOptions::PrintVersion(version);
  REQUIRE(!version.str().empty());

  std::stringstream help;
  pink::CLIOptions::PrintHelp(help);
  REQUIRE(!help.str().empty());
}