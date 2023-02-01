#include "catch2/catch_test_macros.hpp"

#include "aux/StringInterner.h"

TEST_CASE("aux/pink::StringInterner", "[unit][aux]") {
  pink::StringInterner interner;
  std::string          string1 = "some_variable";
  std::string          string2 = "another_variable";

  const auto *interned1 = interner.Intern(string1);
  REQUIRE(interned1 != nullptr);
  REQUIRE(interned1 == string1);

  const auto *interned2 = interner.Intern(string2);
  REQUIRE(interned2 != nullptr);
  REQUIRE(interned2 == string2);
  REQUIRE(interned2 != interned1);
  REQUIRE(interned2 != string1);

  const auto *interned3 = interner.Intern(string1);
  REQUIRE(interned3 != nullptr);
  REQUIRE(interned3 == string1);
  REQUIRE(interned3 == interned1);
  REQUIRE(interned3 != interned2);
  REQUIRE(interned3 != string2);
}