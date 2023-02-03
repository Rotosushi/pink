#include "catch2/catch_test_macros.hpp"

#include "aux/Outcome.h"

TEST_CASE("aux/Outcome", "[unit][aux]") {
  pink::Outcome<int, bool> outcome;

  REQUIRE(outcome.GetWhich() == false);

  SECTION("Outcome can hold first alternative") {
    int integer = 42;
    outcome     = integer;

    REQUIRE(outcome.GetWhich() == true);
    REQUIRE((bool)outcome == true);
    REQUIRE(outcome.GetFirst() == integer);
  }
  SECTION("Outcome can hold second alternative") {
    bool boolean = true;
    outcome      = boolean;

    REQUIRE(outcome.GetWhich() == false);
    REQUIRE((bool)outcome == false);
    REQUIRE(outcome.GetSecond() == boolean);
  }
  SECTION("Outcome can be assigned other type") {
    bool boolean = true;
    int  integer = 42;
    outcome      = boolean;

    REQUIRE(outcome.GetWhich() == false);
    REQUIRE((bool)outcome == false);
    REQUIRE(outcome.GetSecond() == boolean);

    outcome = integer;
    REQUIRE(outcome.GetWhich() == true);
    REQUIRE((bool)outcome == true);
    REQUIRE(outcome.GetFirst() == integer);
  }
  SECTION("Outcome can be assigned another Outcome") {
    bool boolean = false;
    int  integer = 42;
    outcome      = integer;
    REQUIRE(outcome.GetWhich() == true);
    REQUIRE((bool)outcome == true);
    REQUIRE(outcome.GetFirst() == integer);

    pink::Outcome<int, bool> outcome_2(boolean);
    REQUIRE(outcome_2.GetWhich() == false);
    REQUIRE((bool)outcome_2 == false);
    REQUIRE(outcome_2.GetSecond() == boolean);

    outcome = outcome_2;
    REQUIRE(outcome.GetWhich() == false);
    REQUIRE((bool)outcome == false);
    REQUIRE(outcome.GetSecond() == boolean);
  }
}