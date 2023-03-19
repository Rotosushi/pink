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

#include "ast/action/Typecheck.h"

#include "aux/Environment.h"

#include "type/action/ToString.h"

#include "llvm/Support/InitLLVM.h"

static auto ComputeLocation(std::string_view text) {
  return pink::Location{1U, 0U, 1U, text.length() - 1};
}

#define TEST_BIND_TERM_TYPE(bind_text, target_type)                            \
  std::stringstream stream{bind_text};                                         \
  pink::Location    location = ComputeLocation(bind_text);                     \
  env.SetIStream(&stream);                                                     \
  auto parse_result = env.Parse();                                             \
  if (!parse_result) {                                                         \
    env.PrintErrorWithSourceText(std::cerr, parse_result.GetSecond());         \
    FAIL("Unable to parse expression.");                                       \
  }                                                                            \
  REQUIRE(parse_result);                                                       \
  auto &expression = parse_result.GetFirst();                                  \
  CHECK(expression->GetLocation() == location);                                \
  auto typecheck_result = Typecheck(expression, env);                          \
  if (!typecheck_result) {                                                     \
    env.PrintErrorWithSourceText(std::cerr, typecheck_result.GetSecond());     \
    FAIL("Unable to Type expression.");                                        \
  }                                                                            \
  REQUIRE(typecheck_result);                                                   \
  auto term_type = typecheck_result.GetFirst();                                \
  CHECK(term_type == (target_type))

TEST_CASE("ast/action/Typecheck", "[unit][ast][ast/action]") {
  auto env = pink::Environment::CreateTestEnvironment();

  SECTION("a := 42;") { TEST_BIND_TERM_TYPE("a := 42;\n", env.GetIntType()); }

  SECTION("b := true;") {
    TEST_BIND_TERM_TYPE("b := true;\n", env.GetBoolType());
  }

  SECTION("c := (false);") {
    TEST_BIND_TERM_TYPE("c := (false);\n", env.GetBoolType());
  }

  SECTION("d := nil;\n") {
    TEST_BIND_TERM_TYPE("d := nil;\n", env.GetNilType());
  }

  // Causes Error:
  // Type Error: Variable [a] not bound in scope.
  // in spite of the fact that a is being bound in scope
  // manually, and this appears to actually occur when
  // stepping through the code. However, Lookup is failing.
  SECTION("e := a;") {
    env.BindVariable("a", env.GetIntType(), nullptr);
    std::stringstream stream{"e := a;\n"};
    pink::Location    location = ComputeLocation("e := a;\n");
    env.SetIStream(&stream);
    auto parse_result = env.Parse();
    if (!parse_result) {
      env.PrintErrorWithSourceText(std::cerr, parse_result.GetSecond());
      FAIL("Unable to parse expression.");
    }
    REQUIRE(parse_result);
    auto &expression = parse_result.GetFirst();
    CHECK(expression->GetLocation() == location);
    auto typecheck_result = Typecheck(expression, env);
    if (!typecheck_result) {
      env.PrintErrorWithSourceText(std::cerr, typecheck_result.GetSecond());
      FAIL("Unable to Type expression.");
    }
    REQUIRE(typecheck_result);
    auto term_type = typecheck_result.GetFirst();
    CHECK(term_type == (env.GetIntType()));
  }
}

#undef TEST_BIND_TERM_TYPE

/*
#define BIND_TERM_TYPE_IS(bind_term_type)                                      \
  line_number                 += 1;                                            \
  pink::Location location     = locations[line_number - 1];                    \
  auto           parse_result = env.Parse();                                   \
  if (!parse_result) {                                                         \
    env.PrintErrorWithSourceText(std::cerr, parse_result.GetSecond());         \
    FAIL("Unable to parse expression.");                                       \
  }                                                                            \
  REQUIRE(parse_result);                                                       \
  auto &expression = parse_result.GetFirst();                                  \
  CHECK(expression->GetLocation() == location);                                \
  auto typecheck_result = Typecheck(expression, env);                          \
  if (!typecheck_result) {                                                     \
    env.PrintErrorWithSourceText(std::cerr, typecheck_result.GetSecond());     \
    FAIL("Unable to Type expression.");                                        \
  }                                                                            \
  REQUIRE(typecheck_result);                                                   \
  auto term_type = typecheck_result.GetFirst();                                \
  CHECK(term_type == (bind_term_type))

TEST_CASE("ast/action/Typecheck", "[unit][ast][ast/action]") {
  std::vector<std::string_view> source_lines = {
      // basic terms
      "a := 108;\n",
      "b := true;\n",
      "c := (false);\n",
      "d := nil;\n",
      "e := a;\n",
      "f := -36;\n",
      // binops
      "g := 3 + 7;\n",
      "h := (f) + (12);\n",
      // assingment
      "i := b = false;\n",
      // tuple
      "j := (a, b, c, true, e);\n",
      "k := ((a, e), (f, g));\n",
      // array
      "l := [a, e, 2, f, g];\n",
      "m := [[1, 2, 3, 4, 5], [6, 7, 8, 9, 10]];\n",
      // tuple-array mixed
      "n := ([1, 2, 3], [4, 5, 6]);\n",
      "o := [(1, 2), (3, 4), (5, 6)];\n",
      // dot access
      "p := j.0;\n",
      "q := k.1;\n",
      "r := k.0.1;\n",
      // subscript access
      "s := l[3];\n",
      "t := m[0];\n",
      "u := m[1][2];\n",
      // dot-subscript mixed access
      "v := n.0;\n",
      "x := n.0[1];\n",
      "y := o[1];\n",
      "z := o[2].0;\n",
      "aa := j.0 - j.4;\n",
      "ab := k.0.1 + k.1.0;\n",
      "ac := l[1] * l[2];\n",
      "ad := m[0][3] / m[1][2];\n",
      "ae := n.1[0] % n.0[1];\n",
      // address of
      "af := &a;\n",
      "ag := &b;\n",
      "ah := &d;\n",
      // value at
      "ai := *af;\n",

      //"fn f () { nil; }\n",
      //"fn f () { 42; }\n",
      //"fn f () { true; }\n",
      //"fn f () { false; }\n",
      //"fn f () { -23; }\n",
      //"fn f () { 7 * 8; }\n",
      //"fn f () { (5) - (3); }\n",
      //"fn f () { x; }\n",
      //"fn f () { [0, 1, 2, 3, 4]; }\n",
      //"fn f () { (0, 1, 2, 3, 4); }\n",
      //"fn f () { x = 56; }\n",
      //"fn f () { x := 56; }\n",
      //"fn f () { if (true) { 10; } else { 11; } }\n",
      //"fn f () { while (true) do { x = x + 1; } }\n",
      //
      //"fn f (x: Nil) { x; }\n",
      //"fn f (x: Integer) { x; }\n",
      //"fn f (x: Boolean) { x; }\n",
      //"fn f (x: Pointer Integer) { x; }\n",
      //"fn f (x: Slice Integer) { x; }\n",
      //"fn f (x: (Integer, Integer)) { x; }\n",
      //"fn f (x: [Integer; 5]) { x; }\n",
  };
  auto [locations, source] = [&]() {
    std::vector<pink::Location> locations;
    std::string                 buffer;
    unsigned                    line_number = 1;
    for (auto &line : source_lines) {
      locations.emplace_back(line_number, 0U, line_number, line.length() - 1);
      buffer.append(line);
      line_number += 1;
    }
    return std::make_pair(std::move(locations), std::stringstream(buffer));
  }();

  unsigned line_number = 0;
  auto     env         = pink::Environment::CreateTestEnvironment();
  env.SetIStream(&source);

  { BIND_TERM_TYPE_IS(env.GetIntType()); }

  { BIND_TERM_TYPE_IS(env.GetBoolType()); }

  { BIND_TERM_TYPE_IS(env.GetBoolType()); }

  { BIND_TERM_TYPE_IS(env.GetNilType()); }

  { BIND_TERM_TYPE_IS(env.GetIntType()); }

  { BIND_TERM_TYPE_IS(env.GetIntType()); }

  { BIND_TERM_TYPE_IS(env.GetIntType()); }

  { BIND_TERM_TYPE_IS(env.GetIntType()); }

  { BIND_TERM_TYPE_IS(env.GetBoolType()); }

  {
    auto tuple_type = env.GetTupleType({env.GetIntType(),
                                        env.GetBoolType(),
                                        env.GetBoolType(),
                                        env.GetBoolType(),
                                        env.GetIntType()});
    BIND_TERM_TYPE_IS(tuple_type);
  }

  {
    auto tuple_type = env.GetTupleType(
        {env.GetTupleType({env.GetIntType(), env.GetIntType()}),
         env.GetTupleType({env.GetIntType(), env.GetIntType()})});
    BIND_TERM_TYPE_IS(tuple_type);
  }

  { BIND_TERM_TYPE_IS(env.GetArrayType(5U, env.GetIntType())); }

  {
    BIND_TERM_TYPE_IS(
        env.GetArrayType(2U, env.GetArrayType(5U, env.GetIntType())));
  }

  {
    BIND_TERM_TYPE_IS(
        env.GetTupleType({env.GetArrayType(3U, env.GetIntType()),
                          env.GetArrayType(3U, env.GetIntType())}));
  }

  {
    BIND_TERM_TYPE_IS(env.GetArrayType(
        3U,
        env.GetTupleType({env.GetIntType(), env.GetIntType()})));
  }

  { BIND_TERM_TYPE_IS(env.GetIntType()); }

  { BIND_TERM_TYPE_IS(env.GetTupleType({env.GetIntType(), env.GetIntType()})); }

  { BIND_TERM_TYPE_IS(env.GetIntType()); }

  { BIND_TERM_TYPE_IS(env.GetIntType()); }

  { BIND_TERM_TYPE_IS(env.GetArrayType(5U, env.GetIntType())); }

  { BIND_TERM_TYPE_IS(env.GetIntType()); }

  { BIND_TERM_TYPE_IS(env.GetArrayType(3U, env.GetIntType())); }

  { BIND_TERM_TYPE_IS(env.GetIntType()); }
  // "y := o[1];\n" -> (Integer, Integer)
  { BIND_TERM_TYPE_IS(env.GetTupleType({env.GetIntType(), env.GetIntType()})); }

  { BIND_TERM_TYPE_IS(env.GetIntType()); }

  { BIND_TERM_TYPE_IS(env.GetIntType()); }

  { BIND_TERM_TYPE_IS(env.GetIntType()); }

  { BIND_TERM_TYPE_IS(env.GetIntType()); }

  { BIND_TERM_TYPE_IS(env.GetIntType()); }

  { BIND_TERM_TYPE_IS(env.GetIntType()); }

  { BIND_TERM_TYPE_IS(env.GetPointerType(env.GetIntType())); }

  { BIND_TERM_TYPE_IS(env.GetPointerType(env.GetBoolType())); }

  { BIND_TERM_TYPE_IS(env.GetPointerType(env.GetNilType())); }

  { BIND_TERM_TYPE_IS(env.GetIntType()); }
}
*/