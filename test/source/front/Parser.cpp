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

#include "support/ComputeLocation.hpp"
#include "support/LanguageTerms.hpp"

#include "llvm/Support/InitLLVM.h"

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

#define FUNCTION_ARG_TYPE_IS(text, argument_type)                              \
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
  auto &arguments = function->GetArguments();                                  \
  REQUIRE(arguments.size() == 1);                                              \
  REQUIRE(arguments[0].second == (argument_type))

// NOLINTNEXTLINE
TEST_CASE("front/Parser::ParseBind", "[unit][front]") {
  auto env = pink::CompilationUnit::CreateTestEnvironment();
  /*
    Parsing Bind Expressions
  */
  SECTION(term_aaa) { BIND_AFFIX_IS(term_aaa, pink::Integer); }
  SECTION(term_aab) { BIND_AFFIX_IS(term_aab, pink::Boolean); }
  SECTION(term_aac) { BIND_AFFIX_IS(term_aac, pink::Boolean); }
  SECTION(term_aad) { BIND_AFFIX_IS(term_aad, pink::Nil); }
  SECTION(term_aae) { BIND_AFFIX_IS(term_aae, pink::Variable); }
  SECTION(term_aaf) { BIND_AFFIX_IS(term_aaf, pink::Unop); }
  SECTION(term_aag) { BIND_AFFIX_IS(term_aaf, pink::Unop); }
  SECTION(term_aah) { BIND_AFFIX_IS(term_aah, pink::AddressOf); }
  SECTION(term_aai) { BIND_AFFIX_IS(term_aai, pink::ValueOf); }

  SECTION(term_aaj) { BIND_AFFIX_IS(term_aaj, pink::Binop); }
  SECTION(term_aak) { BIND_AFFIX_IS(term_aak, pink::Binop); }
  SECTION(term_aal) { BIND_AFFIX_IS(term_aal, pink::Binop); }
  SECTION(term_aam) { BIND_AFFIX_IS(term_aam, pink::Binop); }
  SECTION(term_aan) { BIND_AFFIX_IS(term_aan, pink::Binop); }
  SECTION(term_aao) { BIND_AFFIX_IS(term_aao, pink::Binop); }
  SECTION(term_aap) { BIND_AFFIX_IS(term_aap, pink::Binop); }
  SECTION(term_aaq) { BIND_AFFIX_IS(term_aaq, pink::Binop); }
  SECTION(term_aar) { BIND_AFFIX_IS(term_aar, pink::Binop); }
  SECTION(term_aas) { BIND_AFFIX_IS(term_aas, pink::Binop); }
  SECTION(term_aat) { BIND_AFFIX_IS(term_aat, pink::Binop); }
  SECTION(term_aau) { BIND_AFFIX_IS(term_aau, pink::Binop); }
  SECTION(term_aav) { BIND_AFFIX_IS(term_aav, pink::Binop); }
  SECTION(term_aaw) { BIND_AFFIX_IS(term_aaw, pink::Binop); }
  SECTION(term_aax) { BIND_AFFIX_IS(term_aax, pink::Binop); }

  SECTION(term_aay) { BIND_AFFIX_IS(term_aay, pink::Binop); }
  SECTION(term_aaz) { BIND_AFFIX_IS(term_aaz, pink::Unop); }
  SECTION(term_aba) { BIND_AFFIX_IS(term_aba, pink::Binop); }
  SECTION(term_abb) { BIND_AFFIX_IS(term_abb, pink::Unop); }
  SECTION(term_abc) { BIND_AFFIX_IS(term_abc, pink::Binop); }
  SECTION(term_abd) { BIND_AFFIX_IS(term_abd, pink::Binop); }

  SECTION(term_abe) { BIND_AFFIX_IS(term_abe, pink::Assignment); }
  SECTION(term_abf) { BIND_AFFIX_IS(term_abf, pink::Dot); }
  SECTION(term_abg) { BIND_AFFIX_IS(term_abg, pink::Subscript); }
  SECTION(term_abh) { BIND_AFFIX_IS(term_abh, pink::Assignment); }
  SECTION(term_abi) { BIND_AFFIX_IS(term_abi, pink::Assignment); }
  SECTION(term_abj) { BIND_AFFIX_IS(term_abj, pink::Binop); }
  SECTION(term_abk) { BIND_AFFIX_IS(term_abk, pink::Binop); }
  SECTION(term_abl) { BIND_AFFIX_IS(term_abl, pink::Dot); }
  SECTION(term_abm) { BIND_AFFIX_IS(term_abm, pink::Subscript); }
  SECTION(term_abn) { BIND_AFFIX_IS(term_abn, pink::Subscript); }
  SECTION(term_abo) { BIND_AFFIX_IS(term_abo, pink::Dot); }
  SECTION(term_abp) { BIND_AFFIX_IS(term_abp, pink::Binop); }
  SECTION(term_abq) { BIND_AFFIX_IS(term_abq, pink::Unop); }
  SECTION(term_abr) { BIND_AFFIX_IS(term_abr, pink::Unop); }

  SECTION(term_abt) { BIND_AFFIX_IS(term_abt, pink::Variable); }
  SECTION(term_abu) { BIND_AFFIX_IS(term_abu, pink::Application); }
  SECTION(term_abv) { BIND_AFFIX_IS(term_abv, pink::Application); }
  SECTION(term_abw) { BIND_AFFIX_IS(term_abw, pink::Application); }
  SECTION(term_abx) { BIND_AFFIX_IS(term_abx, pink::Binop); }
  SECTION(term_aby) { BIND_AFFIX_IS(term_aby, pink::Application); }
  SECTION(term_abz) { BIND_AFFIX_IS(term_abz, pink::Application); }

  SECTION(term_aca) { BIND_AFFIX_IS(term_aca, pink::Array); }
  SECTION(term_acb) { BIND_AFFIX_IS(term_acb, pink::Array); }
  SECTION(term_acc) { BIND_AFFIX_IS(term_acc, pink::Tuple); }
  SECTION(term_acd) { BIND_AFFIX_IS(term_acd, pink::Tuple); }
  SECTION(term_ace) { BIND_AFFIX_IS(term_ace, pink::Array); }
  SECTION(term_acf) { BIND_AFFIX_IS(term_acf, pink::Tuple); }
}

TEST_CASE("front/Parser::ParseFunction", "[unit][front]") {
  auto env = pink::CompilationUnit::CreateTestEnvironment();
  /*
    Parsing Function Definitions
  */
  SECTION(term_acg) { FUNCTION_BODY_IS(term_acg, pink::Integer); }
  SECTION(term_ach) { FUNCTION_BODY_IS(term_ach, pink::Boolean); }
  SECTION(term_aci) { FUNCTION_BODY_IS(term_aci, pink::Boolean); }
  SECTION(term_acj) { FUNCTION_BODY_IS(term_acj, pink::Nil); }
  SECTION(term_ack) { FUNCTION_BODY_IS(term_ack, pink::Unop); }
  SECTION(term_acl) { FUNCTION_BODY_IS(term_acl, pink::Unop); }
  SECTION(term_acm) { FUNCTION_BODY_IS(term_acm, pink::AddressOf); }
  SECTION(term_acn) { FUNCTION_BODY_IS(term_acn, pink::ValueOf); }

  SECTION(term_aco) { FUNCTION_BODY_IS(term_aco, pink::Binop); }
  SECTION(term_acp) { FUNCTION_BODY_IS(term_acp, pink::Binop); }
  SECTION(term_acq) { FUNCTION_BODY_IS(term_acq, pink::Binop); }
  SECTION(term_acr) { FUNCTION_BODY_IS(term_acr, pink::Binop); }
  SECTION(term_acs) { FUNCTION_BODY_IS(term_acs, pink::Binop); }
  SECTION(term_act) { FUNCTION_BODY_IS(term_act, pink::Binop); }
  SECTION(term_acu) { FUNCTION_BODY_IS(term_acu, pink::Binop); }
  SECTION(term_acv) { FUNCTION_BODY_IS(term_acv, pink::Binop); }
  SECTION(term_acw) { FUNCTION_BODY_IS(term_acw, pink::Binop); }
  SECTION(term_acx) { FUNCTION_BODY_IS(term_acx, pink::Binop); }
  SECTION(term_acy) { FUNCTION_BODY_IS(term_acy, pink::Binop); }
  SECTION(term_acz) { FUNCTION_BODY_IS(term_acz, pink::Binop); }
  SECTION(term_ada) { FUNCTION_BODY_IS(term_ada, pink::Binop); }
  SECTION(term_adb) { FUNCTION_BODY_IS(term_adb, pink::Binop); }
  SECTION(term_adc) { FUNCTION_BODY_IS(term_adc, pink::Binop); }

  SECTION(term_add) { FUNCTION_BODY_IS(term_add, pink::Variable); }
  SECTION(term_ade) { FUNCTION_BODY_IS(term_ade, pink::Assignment); }
  SECTION(term_adf) { FUNCTION_BODY_IS(term_adf, pink::Array); }
  SECTION(term_adg) { FUNCTION_BODY_IS(term_adg, pink::Array); }
  SECTION(term_adh) { FUNCTION_BODY_IS(term_adh, pink::Tuple); }
  SECTION(term_adi) { FUNCTION_BODY_IS(term_adi, pink::Tuple); }
  SECTION(term_adj) { FUNCTION_BODY_IS(term_adj, pink::Array); }
  SECTION(term_adk) { FUNCTION_BODY_IS(term_adk, pink::Tuple); }

  SECTION(term_adl) { FUNCTION_BODY_IS(term_adl, pink::Subscript); }
  SECTION(term_adm) { FUNCTION_BODY_IS(term_adm, pink::Subscript); }
  SECTION(term_adn) { FUNCTION_BODY_IS(term_adn, pink::Dot); }
  SECTION(term_ado) { FUNCTION_BODY_IS(term_ado, pink::Dot); }
  SECTION(term_adp) { FUNCTION_BODY_IS(term_adp, pink::Dot); }
  SECTION(term_adq) { FUNCTION_BODY_IS(term_adq, pink::Subscript); }
  SECTION(term_adr) { FUNCTION_BODY_IS(term_adr, pink::Unop); }
  SECTION(term_ads) { FUNCTION_BODY_IS(term_ads, pink::Unop); }

  SECTION(term_adt) { FUNCTION_BODY_IS(term_adt, pink::Variable); }
  SECTION(term_adu) { FUNCTION_BODY_IS(term_adu, pink::Application); }
  SECTION(term_adv) { FUNCTION_BODY_IS(term_adv, pink::Application); }
  SECTION(term_adw) { FUNCTION_BODY_IS(term_adw, pink::Application); }
  SECTION(term_adx) { FUNCTION_BODY_IS(term_adx, pink::Binop); }
  SECTION(term_ady) { FUNCTION_BODY_IS(term_ady, pink::Application); }
  SECTION(term_adz) { FUNCTION_BODY_IS(term_adz, pink::Application); }

  SECTION(term_aea) { FUNCTION_ARG_TYPE_IS(term_aea, env.GetIntType()); }
  SECTION(term_aeb) { FUNCTION_ARG_TYPE_IS(term_aeb, env.GetBoolType()); }
  SECTION(term_aec) { FUNCTION_ARG_TYPE_IS(term_aec, env.GetNilType()); }
  SECTION(term_aed) {
    FUNCTION_ARG_TYPE_IS(term_aed, env.GetPointerType(env.GetIntType()));
  }
  SECTION(term_aee) {
    FUNCTION_ARG_TYPE_IS(term_aee, env.GetSliceType(env.GetIntType()));
  }
  SECTION(term_aef) {
    FUNCTION_ARG_TYPE_IS(
        term_aef,
        env.GetTupleType({env.GetIntType(), env.GetIntType()}));
  }
  SECTION(term_aeg) {
    FUNCTION_ARG_TYPE_IS(term_aeg, env.GetArrayType(10, env.GetIntType()));
  }
  SECTION(term_aeh) {
    FUNCTION_ARG_TYPE_IS(
        term_aeh,
        env.GetPointerType(env.GetPointerType(env.GetIntType())));
  }
  SECTION(term_aei) {
    FUNCTION_ARG_TYPE_IS(
        term_aei,
        env.GetPointerType(env.GetSliceType(env.GetIntType())));
  }
  SECTION(term_aej) {
    FUNCTION_ARG_TYPE_IS(term_aej, env.GetFunctionType(env.GetIntType(), {}));
  }
  SECTION(term_aek) {
    FUNCTION_ARG_TYPE_IS(term_aek, env.GetFunctionType(env.GetIntType(), {}));
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

  { FUNCTION_ARG_TYPE_IS(env.GetNilType()); }

  { FUNCTION_ARG_TYPE_IS(env.GetIntType()); }

  { FUNCTION_ARG_TYPE_IS(env.GetBoolType()); }

  { FUNCTION_ARG_TYPE_IS(env.GetPointerType(env.GetIntType())); }

  { FUNCTION_ARG_TYPE_IS(env.GetSliceType(env.GetIntType())); }

  {
    FUNCTION_ARG_TYPE_IS(
        env.GetTupleType({env.GetIntType(), env.GetIntType()}));
  }

  { FUNCTION_ARG_TYPE_IS(env.GetArrayType(5, env.GetIntType())); }
}
*/
