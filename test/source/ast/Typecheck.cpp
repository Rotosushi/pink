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

#include "support/ComputeLocation.hpp"
#include "support/LanguageTerms.hpp"

#include "llvm/Support/InitLLVM.h"

#define TYPE_IS(bind_text, target_type)                                        \
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
  CHECK(term_type == (target_type));                                           \
  if (term_type != (target_type)) {                                            \
    std::cerr << "actual type [" << ToString(term_type) << "]\n target type [" \
              << ToString(target_type) << "]\n";                               \
  }

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
TEST_CASE("ast/action/Typecheck::Bind", "[unit][ast][ast/action]") {
  auto env = pink::CompilationUnit::CreateTestEnvironment();

  SECTION(term_aaa) { TYPE_IS(term_aaa, env.GetIntType()); }
  SECTION(term_aab) { TYPE_IS(term_aab, env.GetBoolType()); }
  SECTION(term_aac) { TYPE_IS(term_aac, env.GetBoolType()); }
  SECTION(term_aad) { TYPE_IS(term_aad, env.GetNilType()); }
  SECTION(term_aae) {
    env.BindVariable(std::string_view{"a"}, env.GetIntType(), nullptr);
    TYPE_IS(term_aae, env.GetIntType());
  }
  SECTION(term_aaf) { TYPE_IS(term_aaf, env.GetIntType()); }
  SECTION(term_aag) { TYPE_IS(term_aag, env.GetBoolType()); }
  SECTION(term_aah) {
    env.BindVariable(std::string_view{"a"}, env.GetIntType(), nullptr);
    TYPE_IS(term_aah, env.GetPointerType(env.GetIntType()));
  }
  SECTION(term_aai) {
    env.BindVariable(std::string_view{"a"},
                     env.GetPointerType(env.GetIntType()),
                     nullptr);
    TYPE_IS(term_aai, env.GetIntType());
  }

  SECTION(term_aaj) { TYPE_IS(term_aaj, env.GetIntType()); }
  SECTION(term_aak) { TYPE_IS(term_aak, env.GetIntType()); }
  SECTION(term_aal) { TYPE_IS(term_aal, env.GetIntType()); }
  SECTION(term_aam) { TYPE_IS(term_aam, env.GetIntType()); }
  SECTION(term_aan) { TYPE_IS(term_aan, env.GetIntType()); }
  SECTION(term_aao) { TYPE_IS(term_aao, env.GetBoolType()); }
  SECTION(term_aap) { TYPE_IS(term_aap, env.GetBoolType()); }
  SECTION(term_aaq) { TYPE_IS(term_aaq, env.GetBoolType()); }
  SECTION(term_aar) { TYPE_IS(term_aar, env.GetBoolType()); }
  SECTION(term_aas) { TYPE_IS(term_aas, env.GetBoolType()); }
  SECTION(term_aat) { TYPE_IS(term_aat, env.GetBoolType()); }
  SECTION(term_aau) { TYPE_IS(term_aau, env.GetBoolType()); }
  SECTION(term_aav) { TYPE_IS(term_aav, env.GetBoolType()); }
  SECTION(term_aaw) { TYPE_IS(term_aaw, env.GetBoolType()); }
  SECTION(term_aax) { TYPE_IS(term_aax, env.GetBoolType()); }

  SECTION(term_aay) { TYPE_IS(term_aay, env.GetIntType()); }
  SECTION(term_aaz) { TYPE_IS(term_aaz, env.GetIntType()); }
  SECTION(term_aba) { TYPE_IS(term_aba, env.GetBoolType()); }
  SECTION(term_abb) { TYPE_IS(term_abb, env.GetBoolType()); }
  SECTION(term_abc) { TYPE_IS(term_abc, env.GetBoolType()); }
  SECTION(term_abd) { TYPE_IS(term_abd, env.GetBoolType()); }

  SECTION(term_abe) {
    env.BindVariable(std::string_view{"a"}, env.GetIntType(), nullptr);
    TYPE_IS(term_abe, env.GetIntType());
  }
  SECTION(term_abf) {
    env.BindVariable(std::string_view{"tuple"},
                     env.GetTupleType({env.GetIntType(), env.GetIntType()}),
                     nullptr);
    TYPE_IS(term_abf, env.GetIntType());
  }
  SECTION(term_abg) {
    env.BindVariable(std::string_view{"x"}, env.GetIntType(), nullptr);
    env.BindVariable(std::string_view{"array"},
                     env.GetArrayType(5, env.GetIntType()),
                     nullptr);
    TYPE_IS(term_abg, env.GetIntType());
  }
  SECTION(term_abh) {
    env.BindVariable(std::string_view{"tuple"},
                     env.GetTupleType({env.GetIntType(), env.GetIntType()}),
                     nullptr);
    TYPE_IS(term_abh, env.GetIntType());
  }
  SECTION(term_abi) {
    env.BindVariable(std::string_view{"x"}, env.GetIntType(), nullptr);
    env.BindVariable(std::string_view{"array"},
                     env.GetArrayType(5, env.GetIntType()),
                     nullptr);
    TYPE_IS(term_abi, env.GetIntType());
  }
  SECTION(term_abj) {
    env.BindVariable(std::string_view{"tuple"},
                     env.GetTupleType({env.GetIntType(), env.GetIntType()}),
                     nullptr);
    TYPE_IS(term_abj, env.GetIntType());
  }
  SECTION(term_abk) {
    env.BindVariable(std::string_view{"x"}, env.GetIntType(), nullptr);
    env.BindVariable(std::string_view{"array"},
                     env.GetArrayType(5, env.GetIntType()),
                     nullptr);
    TYPE_IS(term_abk, env.GetIntType());
  }
  SECTION(term_abl) {
    auto point = env.GetTupleType({env.GetIntType(), env.GetIntType()});
    env.BindVariable(std::string_view{"tuple"},
                     env.GetTupleType({point, point}),
                     nullptr);
    TYPE_IS(term_abl, env.GetIntType());
  }
  SECTION(term_abm) {
    env.BindVariable(std::string_view{"x"}, env.GetIntType(), nullptr);
    auto array = env.GetArrayType(5, env.GetIntType());
    env.BindVariable(std::string_view{"tuple"},
                     env.GetTupleType({array, array}),
                     nullptr);
    TYPE_IS(term_abm, env.GetIntType());
  }
  SECTION(term_abn) {
    env.BindVariable(std::string_view{"x"}, env.GetIntType(), nullptr);
    env.BindVariable(std::string_view{"y"}, env.GetIntType(), nullptr);
    auto array = env.GetArrayType(5, env.GetIntType());
    env.BindVariable(std::string_view{"array"},
                     env.GetArrayType(5, array),
                     nullptr);
    TYPE_IS(term_abn, env.GetIntType());
  }
  SECTION(term_abo) {
    env.BindVariable(std::string_view{"x"}, env.GetIntType(), nullptr);
    auto tuple = env.GetTupleType({env.GetIntType(), env.GetIntType()});
    env.BindVariable(std::string_view{"array"},
                     env.GetArrayType(5, tuple),
                     nullptr);
    TYPE_IS(term_abo, env.GetIntType());
  }
  SECTION(term_abp) {
    env.BindVariable(std::string_view{"tuple"},
                     env.GetTupleType({env.GetIntType(), env.GetIntType()}),
                     nullptr);
    env.BindVariable(std::string_view{"x"}, env.GetIntType(), nullptr);
    env.BindVariable(std::string_view{"array"},
                     env.GetArrayType(5, env.GetIntType()),
                     nullptr);
    TYPE_IS(term_abp, env.GetIntType());
  }
  SECTION(term_abq) {
    env.BindVariable(std::string_view{"tuple"},
                     env.GetTupleType({env.GetIntType(), env.GetIntType()}),
                     nullptr);
    TYPE_IS(term_abq, env.GetIntType());
  }
  SECTION(term_abr) {
    env.BindVariable(std::string_view{"x"}, env.GetIntType(), nullptr);
    env.BindVariable(std::string_view{"array"},
                     env.GetArrayType(5, env.GetBoolType()),
                     nullptr);
    TYPE_IS(term_abr, env.GetBoolType());
  }

  SECTION(term_abt) {
    env.BindVariable(std::string_view{"f"},
                     env.GetFunctionType(env.GetIntType(), {}),
                     nullptr);
    TYPE_IS(term_abt, env.GetFunctionType(env.GetIntType(), {}));
  }
  SECTION(term_abu) {
    env.BindVariable(std::string_view{"f"},
                     env.GetFunctionType(env.GetIntType(), {}),
                     nullptr);
    TYPE_IS(term_abu, env.GetIntType());
  }
  SECTION(term_abv) {
    env.BindVariable(std::string_view{"f"},
                     env.GetFunctionType(env.GetIntType(), {env.GetIntType()}),
                     nullptr);
    TYPE_IS(term_abv, env.GetIntType());
  }
  SECTION(term_abw) {
    env.BindVariable(std::string_view{"f"},
                     env.GetFunctionType(env.GetIntType(),
                                         {env.GetIntType(), env.GetIntType()}),
                     nullptr);
    TYPE_IS(term_abw, env.GetIntType());
  }
  SECTION(term_abx) {
    env.BindVariable(std::string_view{"f"},
                     env.GetFunctionType(env.GetIntType(),
                                         {env.GetIntType(), env.GetIntType()}),
                     nullptr);
    env.BindVariable(std::string_view{"g"},
                     env.GetFunctionType(env.GetIntType(),
                                         {env.GetIntType(), env.GetIntType()}),
                     nullptr);
    TYPE_IS(term_abx, env.GetIntType());
  }
}

// NOLINTNEXTLINE
TEST_CASE("ast/action/Typecheck::Function", "[unit][ast][ast/action]") {
  auto env = pink::CompilationUnit::CreateTestEnvironment();
}