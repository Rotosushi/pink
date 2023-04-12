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

#include "ops/BinopTable.h"

#include "aux/Environment.h"

auto BinopCodegenFunction([[maybe_unused]] llvm::Value       *left_value,
                          [[maybe_unused]] llvm::Value       *right_value,
                          [[maybe_unused]] pink::Environment &env)
    -> llvm::Value * {
  return nullptr;
}

TEST_CASE("ops/BinopCodegen", "[unit][ops]") {
  auto interner     = pink::TypeInterner{};
  auto integer_type = interner.GetIntType();

  pink::BinopCodegen binop_implementation(integer_type, BinopCodegenFunction);
  REQUIRE(binop_implementation.ReturnType() == integer_type);
}

TEST_CASE("ops/BinopOverloadSet", "[unit][ops]") {
  auto interner     = pink::TypeInterner{};
  auto integer_type = interner.GetIntType();

  pink::BinopOverloadSet binop_literal;
  REQUIRE(binop_literal.Empty());

  auto implementation = binop_literal.Register(integer_type,
                                               integer_type,
                                               integer_type,
                                               BinopCodegenFunction);
  REQUIRE(!binop_literal.Empty());
  REQUIRE(implementation.ReturnType() == integer_type);

  auto found = binop_literal.Lookup(integer_type, integer_type);
  REQUIRE(found.has_value());
  implementation = found.value();
  REQUIRE(implementation.ReturnType() == integer_type);
}

TEST_CASE("ops/BinopTable", "[unit][ops]") {
  auto        interner     = pink::TypeInterner{};
  auto        integer_type = interner.GetIntType();
  pink::Token op           = pink::Token::Add;

  pink::BinopTable binop_table;
  auto             literal = binop_table.Register(op,
                                      integer_type,
                                      integer_type,
                                      integer_type,
                                      BinopCodegenFunction);
  REQUIRE(!literal.Empty());
  auto optional_implementation = literal.Lookup(integer_type, integer_type);
  REQUIRE(optional_implementation.has_value());
  auto implementation = optional_implementation.value();
  REQUIRE(implementation.ReturnType() == integer_type);

  auto optional_literal = binop_table.Lookup(op);
  REQUIRE(optional_literal.has_value());
  literal = optional_literal.value();
  REQUIRE(!literal.Empty());
  optional_implementation = literal.Lookup(integer_type, integer_type);
  REQUIRE(optional_implementation.has_value());
  implementation = optional_implementation.value();
  REQUIRE(implementation.ReturnType() == integer_type);
}