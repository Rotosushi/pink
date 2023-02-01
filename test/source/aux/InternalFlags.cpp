#include "catch2/catch_test_macros.hpp"

#include "aux/InternalFlags.h"

TEST_CASE("aux/pink::InternalFlags", "[unit][aux]") {
  pink::InternalFlags flags;

  REQUIRE(flags.OnTheLHSOfAssignment() == false);
  REQUIRE(flags.WithinAddressOf() == false);
  REQUIRE(flags.WithinBindExpression() == false);
  REQUIRE(flags.WithinDereferencePtr() == false);

  flags.OnTheLHSOfAssignment(true);
  REQUIRE(flags.OnTheLHSOfAssignment() == true);

  flags.WithinAddressOf(true);
  REQUIRE(flags.WithinAddressOf() == true);

  flags.WithinBindExpression(true);
  REQUIRE(flags.WithinBindExpression() == true);

  flags.WithinDereferencePtr(true);
  REQUIRE(flags.WithinDereferencePtr() == true);
}