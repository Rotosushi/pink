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

#include "aux/StringInterner.h"

TEST_CASE("aux/StringInterner", "[unit][aux]") {
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