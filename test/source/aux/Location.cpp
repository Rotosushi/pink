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

#include "aux/Location.h"

#include <random>

TEST_CASE("aux/Location", "[unit][aux]") {
  std::mt19937                          generator;
  std::uniform_int_distribution<size_t> distribution(0);

  for (size_t index = 0; index < 5; index++) {
    size_t         firstLine   = distribution(generator);
    size_t         firstColumn = distribution(generator);
    size_t         lastLine    = distribution(generator);
    size_t         lastColumn  = distribution(generator);
    pink::Location location    = {firstLine, firstColumn, lastLine, lastColumn};
    REQUIRE(location.firstLine == firstLine);
    REQUIRE(location.firstColumn == firstColumn);
    REQUIRE(location.lastLine == lastLine);
    REQUIRE(location.lastColumn == lastColumn);
  }
}