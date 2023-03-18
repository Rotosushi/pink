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

#include "aux/InternalFlags.h"

TEST_CASE("aux/EnvironmentFlags", "[unit][aux]") {
  pink::EnvironmentFlags flags;

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