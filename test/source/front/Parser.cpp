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

#include <iostream>

#include "front/Parser.h"

#include "aux/Environment.h"

#include "ast/All.h"

#include "llvm/Support/InitLLVM.h"

namespace {
static auto ComputeLocation(std::string_view text) -> pink::Location {
  return {1U, 0U, 1U, text.length() - 1};
}
} // namespace

// #TODO: test failure of the parser. (This has been avoided due to
// the semi-frequent changes to the grammar as the language is developed)

// unfortunately I belive these ~must~ be macros due to the
// use of Catch2 macros within their bodies. (free functions
// would not be able to use Catch2 macros properly, and neither
// would lambdas. (I think))
#define BIND_AFFIX_IS(text, affix_type)                                        \
  std::stringstream stream{text};                                              \
  pink::Location    location = ComputeLocation(text);                          \
  env.SetIStream(&stream);                                                     \
  auto parse_result = env.Parse();                                             \
  if (!parse_result) {                                                         \
    env.PrintErrorWithSourceText(std::cerr, parse_result.GetSecond());         \
    FAIL("Unable to parse expression.");                                       \
  }                                                                            \
  REQUIRE(parse_result);                                                       \
  auto *expression = parse_result.GetFirst().get();                            \
  CHECK(expression->GetLocation() == location);                                \
  auto *bind = llvm::dyn_cast<pink::Bind>(expression);                         \
  REQUIRE(bind != nullptr);                                                    \
  auto *affix = bind->GetAffix().get();                                        \
  REQUIRE(llvm::dyn_cast<affix_type>(affix) != nullptr)

#define FUNCTION_BODY_IS(text, body_type)                                      \
  std::stringstream stream{text};                                              \
  pink::Location    location = ComputeLocation(text);                          \
  env.SetIStream(&stream);                                                     \
  auto parse_result = env.Parse();                                             \
  if (!parse_result) {                                                         \
    env.PrintErrorWithSourceText(std::cerr, parse_result.GetSecond());         \
    FAIL("Unable to parse expression.");                                       \
  }                                                                            \
  REQUIRE(parse_result);                                                       \
  auto *expression = parse_result.GetFirst().get();                            \
  CHECK(expression->GetLocation() == location);                                \
  auto *function = llvm::dyn_cast<pink::Function>(expression);                 \
  REQUIRE(function != nullptr);                                                \
  auto *block = llvm::dyn_cast<pink::Block>(function->GetBody().get());        \
  REQUIRE(block != nullptr);                                                   \
  auto cursor = block->begin();                                                \
  REQUIRE(cursor != block->end());                                             \
  REQUIRE(llvm::dyn_cast<body_type>(cursor->get()))

#define FUNCTION_ARGUMENT_TYPE_IS(text, argument_type)                         \
  std::stringstream stream{text};                                              \
  pink::Location    location = ComputeLocation(text);                          \
  env.SetIStream(&stream);                                                     \
  auto parse_result = env.Parse();                                             \
  if (!parse_result) {                                                         \
    env.PrintErrorWithSourceText(std::cerr, parse_result.GetSecond());         \
    FAIL("Unable to parse expression.");                                       \
  }                                                                            \
  REQUIRE(parse_result);                                                       \
  auto *expression = parse_result.GetFirst().get();                            \
  CHECK(expression->GetLocation() == location);                                \
  auto *function = llvm::dyn_cast<pink::Function>(expression);                 \
  REQUIRE(function != nullptr);                                                \
  auto *block = llvm::dyn_cast<pink::Block>(function->GetBody().get());        \
  REQUIRE(block != nullptr);                                                   \
  auto cursor = block->begin();                                                \
  REQUIRE(cursor != block->end());                                             \
  REQUIRE(llvm::dyn_cast<pink::Variable>(cursor->get()));                      \
  auto &arguments = function->GetArguments();                                  \
  REQUIRE(arguments.size() == 1);                                              \
  REQUIRE(arguments[0].second == (argument_type))

// NOLINTNEXTLINE
TEST_CASE("front/Parser::ParseBind", "[unit][front]") {
  auto env = pink::CompilationUnit::CreateTestEnvironment();
  /*
    Parsing Bind Expressions
  */
  SECTION("a := 42;") { BIND_AFFIX_IS("a := 42;\n", pink::Integer); }
  SECTION("b := true;") { BIND_AFFIX_IS("b := true;\n", pink::Boolean); }
  SECTION("c := (false);") { BIND_AFFIX_IS("c := (false);\n", pink::Boolean); }
  SECTION("d := nil;") { BIND_AFFIX_IS("d := nil;\n", pink::Nil); }
  SECTION("e := a;") { BIND_AFFIX_IS("e := a;\n", pink::Variable); }
  SECTION("f := -36;") { BIND_AFFIX_IS("f := -36;\n", pink::Unop); }
  SECTION("g := !true;") { BIND_AFFIX_IS("g := !true;\n", pink::Unop); }
  SECTION("h := &a;") { BIND_AFFIX_IS("h := &a;\n", pink::AddressOf); }
  SECTION("i := *h;") { BIND_AFFIX_IS("i := *h;\n", pink::ValueOf); }
  SECTION("j := 3 + 7;") { BIND_AFFIX_IS("j := 3 + 7;\n", pink::Binop); }
  SECTION("k := (6) - (8);") {
    BIND_AFFIX_IS("k := (6) - (8);\n", pink::Binop);
  }
  SECTION("l := (60 * 10);") {
    BIND_AFFIX_IS("l := (60 * 10);\n", pink::Binop);
  }
  SECTION("m := 48 / 4;") { BIND_AFFIX_IS("m := 48 / 4;\n", pink::Binop); }
  SECTION("n := 77 % 11;") { BIND_AFFIX_IS("n := 77 % 11;\n", pink::Binop); }
  SECTION("o := true & true;") {
    BIND_AFFIX_IS("o := true & true;\n", pink::Binop);
  }
  SECTION("p := 5 == 10;") { BIND_AFFIX_IS("p := 5 == 10;\n", pink::Binop); }
  SECTION("q := 12 != 12;") { BIND_AFFIX_IS("q := 12 != 12;\n", pink::Binop); }
  SECTION("r := 8 < 6;") { BIND_AFFIX_IS("r := 8 < 6;\n", pink::Binop); }
  SECTION("s := 42 <= 56;") { BIND_AFFIX_IS("s := 42 <= 56;\n", pink::Binop); }
  SECTION("t := 56 > 10;") { BIND_AFFIX_IS("t := 56 > 10;\n", pink::Binop); }
  SECTION("u := 21 >= 7;") { BIND_AFFIX_IS("u := 21 >= 7;\n", pink::Binop); }
  SECTION("v := -21 + -4;") { BIND_AFFIX_IS("v := -21 + -4;\n", pink::Binop); }
  SECTION("w := !true | !false;") {
    BIND_AFFIX_IS("w := !true | !false;\n", pink::Binop);
  }
  SECTION("x := a = 46;") { BIND_AFFIX_IS("x := a = 46;\n", pink::Assignment); }
  SECTION("y := [0, 1, 2, 3];") {
    BIND_AFFIX_IS("y := [0, 1, 2, 3];\n", pink::Array);
  }
  SECTION("z := [[0, 1], [2, 3], [4, 5]];") {
    BIND_AFFIX_IS("z := [[0, 1], [2, 3], [4, 5]];\n", pink::Array);
  }
  SECTION("aa := (0, true);") {
    BIND_AFFIX_IS("aa := (0, true);\n", pink::Tuple);
  }
  SECTION("ab := ((0, 0), (1, 1), (2, 2));") {
    BIND_AFFIX_IS("ab := ((0, 0), (1, 1), (2, 2));\n", pink::Tuple);
  }
  SECTION("ac := [(0, 0), (1, 1), (2, 2)];") {
    BIND_AFFIX_IS("ac := [(0, 0), (1, 1), (2, 2)];\n", pink::Array);
  }
  SECTION("ad := ([0, 1, 2, 3], [4, 5, 6, 7]);") {
    BIND_AFFIX_IS("ad := ([0, 1, 2, 3], [4, 5, 6, 7]);\n", pink::Tuple);
  }
  SECTION("ae := y[0];") { BIND_AFFIX_IS("ac := y[0];\n", pink::Subscript); }
  SECTION("af := y[0][1];") {
    BIND_AFFIX_IS("af := y[0][1];\n", pink::Subscript);
  }
  SECTION("ag := aa.0;") { BIND_AFFIX_IS("ag := aa.0;\n", pink::Dot); }
  SECTION("ah := ab.1.1;") { BIND_AFFIX_IS("ah := ab.1.1;\n", pink::Dot); }
  SECTION("ai := ac[0].1;") { BIND_AFFIX_IS("ai := ac[0].1;\n", pink::Dot); }
  SECTION("aj := ad.0[1];") {
    BIND_AFFIX_IS("aj := ad.0[1];\n", pink::Subscript);
  }
  SECTION("ak := !aa.1;") { BIND_AFFIX_IS("ak := !aa.1;\n", pink::Unop); }
  SECTION("al := -aa.0;") { BIND_AFFIX_IS("al := -aa.0;\n", pink::Unop); }
  SECTION("am := f();") { BIND_AFFIX_IS("am := f();\n", pink::Application); }
  SECTION("an := g(0);") { BIND_AFFIX_IS("an := g(0);\n", pink::Application); }
  SECTION("ao := h(0, 1, 2);") {
    BIND_AFFIX_IS("ao := h(0, 1, 2);\n", pink::Application);
  }
  SECTION("ap := f() + g(0);") {
    BIND_AFFIX_IS("ap := f() + g(0);\n", pink::Binop);
  }
  SECTION("aq := tuple.0();") {
    BIND_AFFIX_IS("aq := tuple.0();\n", pink::Application);
  }
  SECTION("ar := array[element]();") {
    BIND_AFFIX_IS("ar := array[element]();\n", pink::Application);
  }
  SECTION("as := array[element] + tuple.0;") {
    BIND_AFFIX_IS("as := array[element] + tuple.0;\n", pink::Binop);
  }
}

TEST_CASE("front/Parser::ParseFunction", "[unit][front]") {
  auto env = pink::CompilationUnit::CreateTestEnvironment();
  /*
    Parsing Function Definitions
  */
  SECTION("fn a() { 42; }") {
    FUNCTION_BODY_IS("fn a() { 42; }\n", pink::Integer);
  }
  SECTION("fn b() { true; }") {
    FUNCTION_BODY_IS("fn b() { true; }\n", pink::Boolean);
  }
  SECTION("fn c() { false; }") {
    FUNCTION_BODY_IS("fn c() { false; }\n", pink::Boolean);
  }
  SECTION("fn d() { nil; }") {
    FUNCTION_BODY_IS("fn d() { nil; }\n", pink::Nil);
  }
  SECTION("fn e() { -1; }") {
    FUNCTION_BODY_IS("fn e() { -1; }\n", pink::Unop);
  }
  SECTION("fn f() { !true; }") {
    FUNCTION_BODY_IS("fn f() { !true; }\n", pink::Unop);
  }
  SECTION("fn g() { &a; }") {
    FUNCTION_BODY_IS("fn g() { &a; }\n", pink::AddressOf);
  }
  SECTION("fn h() { *a; }") {
    FUNCTION_BODY_IS("fn h() { *a; }\n", pink::ValueOf);
  }
  SECTION("fn i() { 1 + 1; }") {
    FUNCTION_BODY_IS("fn i() { 1 + 1; }\n", pink::Binop);
  }
  SECTION("fn j() { 1 - 1; }") {
    FUNCTION_BODY_IS("fn j() { 1 - 1; }\n", pink::Binop);
  }
  SECTION("fn k() { 1 * 1; }") {
    FUNCTION_BODY_IS("fn k() { 1 * 1; }\n", pink::Binop);
  }
  SECTION("fn l() { 1 / 1; }") {
    FUNCTION_BODY_IS("fn l() { 1 / 1; }\n", pink::Binop);
  }
  SECTION("fn m() { 1 % 1; }") {
    FUNCTION_BODY_IS("fn m() { 1 % 1; }\n", pink::Binop);
  }
  SECTION("fn n() { true & true; }") {
    FUNCTION_BODY_IS("fn n() { true & true; }\n", pink::Binop);
  }
  SECTION("fn o() { false | true; }") {
    FUNCTION_BODY_IS("fn o() { false | true; }\n", pink::Binop);
  }
  SECTION("fn p() { 0 == 1; }") {
    FUNCTION_BODY_IS("fn p() { 0 == 1; }\n", pink::Binop);
  }
  SECTION("fn q() { 0 != 1; }") {
    FUNCTION_BODY_IS("fn q() { 0 != 1; }\n", pink::Binop);
  }
  SECTION("fn r() { 10 < 100; }") {
    FUNCTION_BODY_IS("fn r() { 10 < 100; }\n", pink::Binop);
  }
  SECTION("fn s() { 10 <= 11; }") {
    FUNCTION_BODY_IS("fn s() { 10 <= 11; }\n", pink::Binop);
  }
  SECTION("fn t() { 10 > 10; }") {
    FUNCTION_BODY_IS("fn t() { 10 > 10; }\n", pink::Binop);
  }
  SECTION("fn u() { 10 >= 10; }") {
    FUNCTION_BODY_IS("fn u() { 10 >= 10; }\n", pink::Binop);
  }
  SECTION("fn v() { a; }") {
    FUNCTION_BODY_IS("fn v() { a; }\n", pink::Variable);
  }
  SECTION("fn w() { a = b; }") {
    FUNCTION_BODY_IS("fn w() { a = b; }\n", pink::Assignment);
  }
  SECTION("fn x() { [0, 1, 2, 3, 4] }") {
    FUNCTION_BODY_IS("fn x() { [0, 1, 2, 3, 4]; }\n", pink::Array);
  }
  SECTION("fn y() { [[0,1], [2,3], [4,5]]; }") {
    FUNCTION_BODY_IS("fn y() { [[0,1], [2,3], [4,5]]; }\n", pink::Array);
  }
  SECTION("fn z() { (0, true); }") {
    FUNCTION_BODY_IS("fn y() { (0, true); }\n", pink::Tuple);
  }
  SECTION("fn aa() { ((0, 0), (1, 1), (2, 2)); }") {
    FUNCTION_BODY_IS("fn aa() { ((0, 0), (1, 1), (2, 2)); }\n", pink::Tuple);
  }
  SECTION("fn ab() { [(0, 0), (1, 1), (2, 2)]; }") {
    FUNCTION_BODY_IS("fn ab() { [(0, 0), (1, 1), (2, 2)]; }\n", pink::Array);
  }
  SECTION("fn ac() { ([0, 1, 2, 3], [4, 5, 6, 7]); }") {
    FUNCTION_BODY_IS("fn ac() { ([0, 1, 2, 3], [4, 5, 6, 7]); }\n",
                     pink::Tuple);
  }
  SECTION("fn ad() { array[element]; }") {
    FUNCTION_BODY_IS("fn ad() { array[element]; }\n", pink::Subscript);
  }
  SECTION("fn ae() { array[element][element]; }") {
    FUNCTION_BODY_IS("fn ae() { array[element][element]; }\n", pink::Subscript);
  }
  SECTION("fn af() { tuple.member; }") {
    FUNCTION_BODY_IS("fn af() { tuple.member; }\n", pink::Dot);
  }
  SECTION("fn ag() { tuple.member.member; }") {
    FUNCTION_BODY_IS("fn ag() { tuple.member.member; }\n", pink::Dot);
  }
  SECTION("fn ah() { array[element].member; }") {
    FUNCTION_BODY_IS("fn ah() { array[element].member; }\n", pink::Dot);
  }
  SECTION("fn ai() { tuple.member[element]; }") {
    FUNCTION_BODY_IS("fn ai() { tuple.member[element]; }\n", pink::Subscript);
  }
  SECTION("fn aj() { !tuple.member; }") {
    FUNCTION_BODY_IS("fn aj() { !tuple.member; }\n", pink::Unop);
  }
  SECTION("fn ak() { -array[element]; }") {
    FUNCTION_BODY_IS("fn ak() { -array[element]; }\n", pink::Unop);
  }
  SECTION("fn al() { function(); }") {
    FUNCTION_BODY_IS("fn al() { function(); }\n", pink::Application);
  }
  SECTION("fn am() { function(0); }") {
    FUNCTION_BODY_IS("fn am() { function(0); }\n", pink::Application);
  }
  SECTION("fn an() { function(0, 1, 2); }") {
    FUNCTION_BODY_IS("fn an() { function(0, 1, 2); }\n", pink::Application);
  }
  SECTION("fn ao() { f(1) + g(2, 3); }") {
    FUNCTION_BODY_IS("fn ao() { f(1) + g(2, 3); }\n", pink::Binop);
  }
  SECTION("fn ap() { tuple.member(); }") {
    FUNCTION_BODY_IS("fn ap() { tuple.member(); }\n", pink::Application);
  }
  SECTION("fn aq() { array[element](); }") {
    FUNCTION_BODY_IS("fn aq() { array[element](); }\n", pink::Application);
  }
  SECTION("fn ar() { array[element] + tuple.member; }") {
    FUNCTION_BODY_IS("fn ar() { array[element] + tuple.member; }\n",
                     pink::Binop);
  }
  /*
    Parsing argument types
  */
  SECTION("fn as(a: Integer) { a; }") {
    FUNCTION_ARGUMENT_TYPE_IS("fn as(a: Integer) { a; }\n", env.GetIntType());
  }
  SECTION("fn at(a: Boolean) { a; }") {
    FUNCTION_ARGUMENT_TYPE_IS("fn at(a: Boolean) { a; }\n", env.GetBoolType());
  }
  SECTION("fn au(a: Nil) { a; }") {
    FUNCTION_ARGUMENT_TYPE_IS("fn au(a: Nil) { a; }\n", env.GetNilType());
  }
  SECTION("fn av(a: *Integer) { a; }") {
    FUNCTION_ARGUMENT_TYPE_IS("fn av(a: *Integer) { a; }\n",
                              env.GetPointerType(env.GetIntType()));
  }
  SECTION("fn aw(a: *[]Integer) { a; }") {
    FUNCTION_ARGUMENT_TYPE_IS("fn aw(a: *[]Integer) { a; }\n",
                              env.GetSliceType(env.GetIntType()));
  }
  SECTION("fn ax(a: (Integer, Integer)) { a; }") {
    FUNCTION_ARGUMENT_TYPE_IS(
        "fn ax(a: (Integer, Integer)) { a; }\n",
        env.GetTupleType({env.GetIntType(), env.GetIntType()}));
  }
  SECTION("fn ay(a: [Integer; 10]) { a; }") {
    FUNCTION_ARGUMENT_TYPE_IS("fn ay(a: [Integer; 10]) { a; }\n",
                              env.GetArrayType(10, env.GetIntType()));
  }
  SECTION("fn az(a: **Integer) { a; }") {
    FUNCTION_ARGUMENT_TYPE_IS(
        "fn az(a: **Integer) { a; }\n",
        env.GetPointerType(env.GetPointerType(env.GetIntType())));
  }
  SECTION("fn ba(a: **[]Integer) { a; }") {
    FUNCTION_ARGUMENT_TYPE_IS(
        "fn ba(a: **[]Integer) { a; }\n",
        env.GetPointerType(env.GetSliceType(env.GetIntType())));
  }
  SECTION("fn bb(a: fn() -> Integer) { a; }") {
    FUNCTION_ARGUMENT_TYPE_IS("fn bb(a: fn() -> Integer) { a; }\n",
                              env.GetFunctionType(env.GetIntType(), {}));
  }
}

/*
TEST_CASE("front/Parser", "[unit][front]") {
    The general idea is to ensure that all valid terms
    of the langauge we want to be semanitcally meaninfull,
    we want to be able to parse, so this *should* be a
    list of all valid forms in the language. (it probably
    isn't and will be updated with new and interesting test cases.)

    note that since we are just testing that everything Parses,
    we do not need to adhere to the typechecking rules.

  std::vector<std::string_view> source_lines = {
      "a := 108;\n",
      "a := true;\n",
      "a := false;\n",
      "a := nil;\n",
      "a := x;\n",
      "a := -36;\n",
      "a := 3 + 7;\n",
      "a := (0) + (5);\n",
      "a := [0, 1, 2, 3, 4];\n",
      "a := (0, 1, 2, 3, 4);\n",
      "a := b = (true, false);\n",
      "a := b.0;\n",
      "a := b.0.1;\n",
      "a := b[0];\n",
      "a := b[0][1];\n",
      "a := b[0].1;\n",
      "a := b.1[2];\n",
      "a := b.1 + c[x];\n",

      "fn f () { nil; }\n",
      "fn f () { 42; }\n",
      "fn f () { true; }\n",
      "fn f () { false; }\n",
      "fn f () { -23; }\n",
      "fn f () { 7 * 8; }\n",
      "fn f () { (5) - (3); }\n",
      "fn f () { x; }\n",
      "fn f () { [0, 1, 2, 3, 4]; }\n",
      "fn f () { (0, 1, 2, 3, 4); }\n",
      "fn f () { x = 56; }\n",
      "fn f () { x := 56; }\n",
      "fn f () { if (true) { 10; } else { 11; } }\n",
      "fn f () { while (true) do { x = x + 1; } }\n",

      "fn f (x: Nil) { x; }\n",
      "fn f (x: Integer) { x; }\n",
      "fn f (x: Boolean) { x; }\n",
      "fn f (x: Pointer Integer) { x; }\n",
      "fn f (x: Slice Integer) { x; }\n",
      "fn f (x: (Integer, Integer)) { x; }\n",
      "fn f (x: [Integer; 5]) { x; }\n",
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

  { BIND_AFFIX_IS(pink::Integer); }

  { BIND_AFFIX_IS(pink::Boolean); }

  { BIND_AFFIX_IS(pink::Boolean); }

  { BIND_AFFIX_IS(pink::Nil); }

  { BIND_AFFIX_IS(pink::Variable); }

  { BIND_AFFIX_IS(pink::Unop); }

  { BIND_AFFIX_IS(pink::Binop); }

  { BIND_AFFIX_IS(pink::Binop); }

  { BIND_AFFIX_IS(pink::Array); }

  { BIND_AFFIX_IS(pink::Tuple); }

  { BIND_AFFIX_IS(pink::Assignment); }

  { BIND_AFFIX_IS(pink::Dot); }

  { BIND_AFFIX_IS(pink::Dot); }

  { BIND_AFFIX_IS(pink::Subscript); }

  { BIND_AFFIX_IS(pink::Subscript); }

  { BIND_AFFIX_IS(pink::Dot); }

  { BIND_AFFIX_IS(pink::Subscript); }

  { BIND_AFFIX_IS(pink::Binop); }

  { FUNCTION_BODY_IS(pink::Nil); }

  { FUNCTION_BODY_IS(pink::Integer); }

  { FUNCTION_BODY_IS(pink::Boolean); }

  { FUNCTION_BODY_IS(pink::Boolean); }

  { FUNCTION_BODY_IS(pink::Unop); }

  { FUNCTION_BODY_IS(pink::Binop); }

  { FUNCTION_BODY_IS(pink::Binop); }

  { FUNCTION_BODY_IS(pink::Variable); }

  { FUNCTION_BODY_IS(pink::Array); }

  { FUNCTION_BODY_IS(pink::Tuple); }

  { FUNCTION_BODY_IS(pink::Assignment); }

  { FUNCTION_BODY_IS(pink::Bind); }

  { FUNCTION_BODY_IS(pink::IfThenElse); }

  { FUNCTION_BODY_IS(pink::While); }

  { FUNCTION_ARGUMENT_TYPE_IS(env.GetNilType()); }

  { FUNCTION_ARGUMENT_TYPE_IS(env.GetIntType()); }

  { FUNCTION_ARGUMENT_TYPE_IS(env.GetBoolType()); }

  { FUNCTION_ARGUMENT_TYPE_IS(env.GetPointerType(env.GetIntType())); }

  { FUNCTION_ARGUMENT_TYPE_IS(env.GetSliceType(env.GetIntType())); }

  {
    FUNCTION_ARGUMENT_TYPE_IS(
        env.GetTupleType({env.GetIntType(), env.GetIntType()}));
  }

  { FUNCTION_ARGUMENT_TYPE_IS(env.GetArrayType(5, env.GetIntType())); }
}
*/
