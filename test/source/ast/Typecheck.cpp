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

// #REASON Normally I agree with this metric. and avoid macros
// like the plague, however this is a test
// function, and all of the complexity comes from repeating the
// same test lines on differing inputs. Thus the function is as
// 'complex' as a series of function calls with distinct parameters.
// and would literally be a series of function calls were it not for catch2.
// the only reason this function is flagged is due to the use of
// macros, which explicitly inline the code. and we need to use
// the macro to remove the repeated code due to the macros that
// CATCH2 uses. these macros need to be expanded within the scope
// of the TEST_CASE. thus we are forced to use a macro here.
// thus the code is physically repeated, thus clang tidy complains
// that this function is too complex. when it really isn't.
// NOLINTNEXTLINE(readability-function-cognitive-complexity)
TEST_CASE("ast/action/TypecheckBind", "[unit][ast][ast/action]") {
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

  SECTION("e := a;") {
    env.BindVariable(std::string_view{"a"}, env.GetIntType(), nullptr);
    TEST_BIND_TERM_TYPE("e := a;\n", env.GetIntType());
  }

  SECTION("f := -36;") { TEST_BIND_TERM_TYPE("f := -36;\n", env.GetIntType()); }

  SECTION("g := 3 + 7;") {
    TEST_BIND_TERM_TYPE("g := 3 + 7;\n", env.GetIntType());
  }

  SECTION("h := ((f) + (12));") {
    env.BindVariable(std::string_view{"f"}, env.GetIntType(), nullptr);
    TEST_BIND_TERM_TYPE("h := (f) + (12);\n", env.GetIntType());
  }

  SECTION("i := 5 - 10;") {
    TEST_BIND_TERM_TYPE("i := 5 - 10;\n", env.GetIntType());
  }

  SECTION("j := 30 * 7;") {
    TEST_BIND_TERM_TYPE("j := 30 * 7;\n", env.GetIntType());
  }

  SECTION("k := 81 / 3;") {
    TEST_BIND_TERM_TYPE("k := 81 / 3;\n", env.GetIntType());
  }

  SECTION("l := 45 % 8;") {
    std::stringstream stream{"l := 45 % 8;\n"};
    pink::Location    location = ComputeLocation("l := 45 % 8;\n");
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

  SECTION("m := true & true;") {
    TEST_BIND_TERM_TYPE("m := true & true;\n", env.GetBoolType());
  }

  SECTION("n := true | false;") {
    TEST_BIND_TERM_TYPE("n := true | false;\n", env.GetBoolType());
  }

  SECTION("o := 5 == 5;") {
    TEST_BIND_TERM_TYPE("o := 5 == 5;\n", env.GetBoolType());
  }

  SECTION("p := 12 != 102;") {
    TEST_BIND_TERM_TYPE("p := 12 != 102;\n", env.GetBoolType());
  }

  SECTION("q := 8 < 6;") {
    TEST_BIND_TERM_TYPE("q := 8 < 6;\n", env.GetBoolType());
  }

  SECTION("r := 42 <= 42;") {
    TEST_BIND_TERM_TYPE("r := 42 <= 42;\n", env.GetBoolType());
  }

  SECTION("s := 56 > 10;") {
    TEST_BIND_TERM_TYPE("s := 56 > 10;\n", env.GetBoolType());
  }

  SECTION("t := 21 >= 7;") {
    TEST_BIND_TERM_TYPE("t := 21 >= 7;\n", env.GetBoolType());
  }

  SECTION("u := -21 + -7;") {
    TEST_BIND_TERM_TYPE("u := -21 + -7;\n", env.GetIntType());
  }

  SECTION("v := !true | !false;") {
    TEST_BIND_TERM_TYPE("v := !true | !false;\n", env.GetBoolType());
  }

  SECTION("w := a = 53;") {
    env.BindVariable(std::string_view{"a"}, env.GetIntType(), nullptr);
    TEST_BIND_TERM_TYPE("w := a = 53;\n", env.GetIntType());
  }

  SECTION("x := &a;") {
    env.BindVariable(std::string_view{"a"}, env.GetIntType(), nullptr);
    TEST_BIND_TERM_TYPE("x := &a;\n", env.GetPointerType(env.GetIntType()));
  }

  SECTION("y := &b;") {
    env.BindVariable(std::string_view{"b"}, env.GetBoolType(), nullptr);
    TEST_BIND_TERM_TYPE("y := &b;\n", env.GetPointerType(env.GetBoolType()));
  }

  // causes error: somehow the return type is equal to the argument type.
  // swapping the order of the arguments within RegisterTemplateBuiltinUnop
  // only causes the & operator to also fail in the same way, yet
  // in both cases the * operator fails
  SECTION("z := *x;") {
    env.BindVariable(std::string_view{"x"},
                     env.GetPointerType(env.GetIntType()),
                     nullptr);
    std::stringstream stream{"z := *x;\n"};
    pink::Location    location = ComputeLocation("z := *x;\n");
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

  SECTION("aa := *y;") {
    env.BindVariable(std::string_view{"y"},
                     env.GetPointerType(env.GetBoolType()),
                     nullptr);
    TEST_BIND_TERM_TYPE("aa := *y;\n", env.GetBoolType());
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