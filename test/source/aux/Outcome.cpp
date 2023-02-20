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