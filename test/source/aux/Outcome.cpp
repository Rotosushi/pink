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

#include "aux/Outcome.h"

TEST_CASE("aux/Outcome", "[unit][aux]") {
  pink::Outcome<int, bool> outcome;

  REQUIRE(outcome.GetWhich() == false);
  REQUIRE(outcome == false);

  SECTION("Outcome can hold first alternative") {
    const int integer = 42;
    outcome           = integer;

    REQUIRE(outcome.GetWhich() == true);
    REQUIRE(outcome == true);
    REQUIRE(outcome.GetFirst() == integer);
  }
  SECTION("Outcome can hold second alternative") {
    const bool boolean = true;
    outcome            = boolean;

    REQUIRE(outcome.GetWhich() == false);
    REQUIRE(outcome == false);
    REQUIRE(outcome.GetSecond() == boolean);
  }
  SECTION("Outcome can be assigned other type") {
    const bool boolean = true;
    const int  integer = 42;
    outcome            = boolean;

    REQUIRE(outcome.GetWhich() == false);
    REQUIRE(outcome == false);
    REQUIRE(outcome.GetSecond() == boolean);

    outcome = integer;
    REQUIRE(outcome.GetWhich() == true);
    REQUIRE(outcome == true);
    REQUIRE(outcome.GetFirst() == integer);
  }
  SECTION("Outcome can be assigned another Outcome") {
    const bool boolean = false;
    const int  integer = 42;
    outcome            = integer;
    REQUIRE(outcome.GetWhich() == true);
    REQUIRE(outcome == true);
    REQUIRE(outcome.GetFirst() == integer);

    pink::Outcome<int, bool> outcome_2(boolean);
    REQUIRE(outcome_2.GetWhich() == false);
    REQUIRE(outcome_2 == false);
    REQUIRE(outcome_2.GetSecond() == boolean);

    outcome = outcome_2;
    REQUIRE(outcome.GetWhich() == false);
    REQUIRE(outcome == false);
    REQUIRE(outcome.GetSecond() == boolean);
  }
}