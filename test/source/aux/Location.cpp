#include "catch2/catch_test_macros.hpp"

#include "aux/Location.h"

#include <random>

TEST_CASE("aux/Location", "[unit][aux]") {
  std::mt19937                          generator;
  std::uniform_int_distribution<size_t> distribution(0);

  SECTION("Location holds given values", "[unit]") {
    for (size_t index = 0; index < 5; index++) {
      size_t         firstLine   = distribution(generator);
      size_t         firstColumn = distribution(generator);
      size_t         lastLine    = distribution(generator);
      size_t         lastColumn  = distribution(generator);
      pink::Location location = {firstLine, firstColumn, lastLine, lastColumn};
      REQUIRE(location.firstLine == firstLine);
      REQUIRE(location.firstColumn == firstColumn);
      REQUIRE(location.lastLine == lastLine);
      REQUIRE(location.lastColumn == lastColumn);
    }
  }
}