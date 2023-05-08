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

#include "aux/Environment.h"

#include "support/ComputeLocation.hpp"
#include "support/LanguageTerms.hpp"
#include "support/TestTerms.hpp"

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
  CHECK(Equals(term_type, (target_type)));                                     \
  if (!Equals(term_type, (target_type))) {                                     \
    std::cerr << "actual type [" << term_type << "]\n target type ["           \
              << (target_type) << "]\n";                                       \
  }

template <typename T>
inline static auto TypecheckTerm(TermGenFn<T> fn) -> bool {
  bool result   = true;
  auto unit     = pink::CompilationUnit::CreateTestCompilationUnit();
  auto expected = fn(unit);
  auto location = ComputeLocation(expected.term.str());
  unit.SetIStream(&expected.term);

  auto parse_outcome = unit.Parse();
  if (!parse_outcome) {
    unit.PrintErrorWithSourceText(std::cerr, parse_outcome.GetSecond());
    return false;
  }
  auto &expression = parse_outcome.GetFirst();

  if (expression->GetLocation() != location) {
    std::cerr << "actual location " << expression->GetLocation()
              << "\n expected location " << location << "\n";
    // location information being incorrect
    // does not stop type information from
    // being created and checked, so this
    // merely fails the test, it doesn't
    // cause early return.
    result = false;
  }

  auto typecheck_outcome = expression->Typecheck(unit);
  if (!typecheck_outcome) {
    unit.PrintErrorWithSourceText(std::cerr, typecheck_outcome.GetSecond());
    return false;
  }
  auto type = typecheck_outcome.GetFirst();

  auto equal = type->Equals(expected.type);
  if (!equal) {
    std::cerr << "actual type [" << type << "]\n target type [" << expected.type
              << "]\n";
    result = false;
  }

  return result;
}

TEST_CASE("Typecheck: Term0001", "[unit]") {
  auto test = TypecheckTerm(Term0001);
  REQUIRE(test);
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
  pink::Type::Annotations annotations;
  annotations.IsInMemory(false);
  auto env = pink::CompilationUnit::CreateTestCompilationUnit();

  SECTION(term_aaa) { TYPE_IS(term_aaa, env.GetIntType(annotations)); }
  SECTION(term_aab) { TYPE_IS(term_aab, env.GetBoolType(annotations)); }
  SECTION(term_aac) { TYPE_IS(term_aac, env.GetBoolType(annotations)); }
  SECTION(term_aad) { TYPE_IS(term_aad, env.GetNilType(annotations)); }
  SECTION(term_aae) {
    env.BindVariable(std::string_view{"a"},
                     env.GetIntType(annotations),
                     nullptr);
    TYPE_IS(term_aae, env.GetIntType(annotations));
  }
  SECTION(term_aaf) { TYPE_IS(term_aaf, env.GetIntType(annotations)); }
  SECTION(term_aag) { TYPE_IS(term_aag, env.GetBoolType(annotations)); }
  SECTION(term_aah) {
    annotations.IsInMemory(true);
    env.BindVariable(std::string_view{"a"},
                     env.GetIntType(annotations),
                     nullptr);
    TYPE_IS(term_aah,
            env.GetPointerType(annotations, env.GetIntType(annotations)));
  }
  SECTION(term_aai) {
    env.BindVariable(
        std::string_view{"a"},
        env.GetPointerType(annotations, env.GetIntType(annotations)),
        nullptr);
    TYPE_IS(term_aai, env.GetIntType(annotations));
  }

  SECTION(term_aaj) { TYPE_IS(term_aaj, env.GetIntType(annotations)); }
  SECTION(term_aak) { TYPE_IS(term_aak, env.GetIntType(annotations)); }
  SECTION(term_aal) { TYPE_IS(term_aal, env.GetIntType(annotations)); }
  SECTION(term_aam) { TYPE_IS(term_aam, env.GetIntType(annotations)); }
  SECTION(term_aan) { TYPE_IS(term_aan, env.GetIntType(annotations)); }
  SECTION(term_aao) { TYPE_IS(term_aao, env.GetBoolType(annotations)); }
  SECTION(term_aap) { TYPE_IS(term_aap, env.GetBoolType(annotations)); }
  SECTION(term_aaq) { TYPE_IS(term_aaq, env.GetBoolType(annotations)); }
  SECTION(term_aar) { TYPE_IS(term_aar, env.GetBoolType(annotations)); }
  SECTION(term_aas) { TYPE_IS(term_aas, env.GetBoolType(annotations)); }
  SECTION(term_aat) { TYPE_IS(term_aat, env.GetBoolType(annotations)); }
  SECTION(term_aau) { TYPE_IS(term_aau, env.GetBoolType(annotations)); }
  SECTION(term_aav) { TYPE_IS(term_aav, env.GetBoolType(annotations)); }
  SECTION(term_aaw) { TYPE_IS(term_aaw, env.GetBoolType(annotations)); }
  SECTION(term_aax) { TYPE_IS(term_aax, env.GetBoolType(annotations)); }

  SECTION(term_aay) { TYPE_IS(term_aay, env.GetIntType(annotations)); }
  SECTION(term_aaz) { TYPE_IS(term_aaz, env.GetIntType(annotations)); }
  SECTION(term_aba) { TYPE_IS(term_aba, env.GetBoolType(annotations)); }
  SECTION(term_abb) { TYPE_IS(term_abb, env.GetBoolType(annotations)); }
  SECTION(term_abc) { TYPE_IS(term_abc, env.GetBoolType(annotations)); }
  SECTION(term_abd) { TYPE_IS(term_abd, env.GetBoolType(annotations)); }

  SECTION(term_abe) {
    env.BindVariable(std::string_view{"a"},
                     env.GetIntType(annotations),
                     nullptr);
    TYPE_IS(term_abe, env.GetIntType(annotations));
  }
  SECTION(term_abf) {
    env.BindVariable(std::string_view{"tuple"},
                     env.GetTupleType(annotations,
                                      {env.GetIntType(annotations),
                                       env.GetIntType(annotations)}),
                     nullptr);
    TYPE_IS(term_abf, env.GetIntType(annotations));
  }
  SECTION(term_abg) {
    env.BindVariable(std::string_view{"x"},
                     env.GetIntType(annotations),
                     nullptr);
    env.BindVariable(
        std::string_view{"array"},
        env.GetArrayType(annotations, 5, env.GetIntType(annotations)),
        nullptr);
    TYPE_IS(term_abg, env.GetIntType(annotations));
  }
  SECTION(term_abh) {
    env.BindVariable(std::string_view{"tuple"},
                     env.GetTupleType(annotations,
                                      {env.GetIntType(annotations),
                                       env.GetIntType(annotations)}),
                     nullptr);
    TYPE_IS(term_abh, env.GetIntType(annotations));
  }
  SECTION(term_abi) {
    env.BindVariable(std::string_view{"x"},
                     env.GetIntType(annotations),
                     nullptr);
    env.BindVariable(
        std::string_view{"array"},
        env.GetArrayType(annotations, 5, env.GetIntType(annotations)),
        nullptr);
    TYPE_IS(term_abi, env.GetIntType(annotations));
  }
  SECTION(term_abj) {
    env.BindVariable(std::string_view{"tuple"},
                     env.GetTupleType(annotations,
                                      {env.GetIntType(annotations),
                                       env.GetIntType(annotations)}),
                     nullptr);
    TYPE_IS(term_abj, env.GetIntType(annotations));
  }
  SECTION(term_abk) {
    env.BindVariable(std::string_view{"x"},
                     env.GetIntType(annotations),
                     nullptr);
    env.BindVariable(
        std::string_view{"array"},
        env.GetArrayType(annotations, 5, env.GetIntType(annotations)),
        nullptr);
    TYPE_IS(term_abk, env.GetIntType(annotations));
  }
  SECTION(term_abl) {
    auto point = env.GetTupleType(
        annotations,
        {env.GetIntType(annotations), env.GetIntType(annotations)});
    env.BindVariable(std::string_view{"tuple"},
                     env.GetTupleType(annotations, {point, point}),
                     nullptr);
    TYPE_IS(term_abl, env.GetIntType(annotations));
  }
  SECTION(term_abm) {
    env.BindVariable(std::string_view{"x"},
                     env.GetIntType(annotations),
                     nullptr);
    auto array = env.GetArrayType(annotations, 5, env.GetIntType(annotations));
    env.BindVariable(std::string_view{"tuple"},
                     env.GetTupleType(annotations, {array, array}),
                     nullptr);
    TYPE_IS(term_abm, env.GetIntType(annotations));
  }
  SECTION(term_abn) {
    env.BindVariable(std::string_view{"x"},
                     env.GetIntType(annotations),
                     nullptr);
    env.BindVariable(std::string_view{"y"},
                     env.GetIntType(annotations),
                     nullptr);
    auto array = env.GetArrayType(annotations, 5, env.GetIntType(annotations));
    env.BindVariable(std::string_view{"array"},
                     env.GetArrayType(annotations, 5, array),
                     nullptr);
    TYPE_IS(term_abn, env.GetIntType(annotations));
  }
  SECTION(term_abo) {
    env.BindVariable(std::string_view{"x"},
                     env.GetIntType(annotations),
                     nullptr);
    auto tuple = env.GetTupleType(
        annotations,
        {env.GetIntType(annotations), env.GetIntType(annotations)});
    env.BindVariable(std::string_view{"array"},
                     env.GetArrayType(annotations, 5, tuple),
                     nullptr);
    TYPE_IS(term_abo, env.GetIntType(annotations));
  }
  SECTION(term_abp) {
    env.BindVariable(std::string_view{"tuple"},
                     env.GetTupleType(annotations,
                                      {env.GetIntType(annotations),
                                       env.GetIntType(annotations)}),
                     nullptr);
    env.BindVariable(std::string_view{"x"},
                     env.GetIntType(annotations),
                     nullptr);
    env.BindVariable(
        std::string_view{"array"},
        env.GetArrayType(annotations, 5, env.GetIntType(annotations)),
        nullptr);
    TYPE_IS(term_abp, env.GetIntType(annotations));
  }
  SECTION(term_abq) {
    env.BindVariable(std::string_view{"tuple"},
                     env.GetTupleType(annotations,
                                      {env.GetIntType(annotations),
                                       env.GetIntType(annotations)}),
                     nullptr);
    TYPE_IS(term_abq, env.GetIntType(annotations));
  }
  SECTION(term_abr) {
    env.BindVariable(std::string_view{"x"},
                     env.GetIntType(annotations),
                     nullptr);
    env.BindVariable(
        std::string_view{"array"},
        env.GetArrayType(annotations, 5, env.GetBoolType(annotations)),
        nullptr);
    TYPE_IS(term_abr, env.GetBoolType(annotations));
  }

  SECTION(term_abt) {
    env.BindVariable(
        std::string_view{"f"},
        env.GetFunctionType(annotations, env.GetIntType(annotations), {}),
        nullptr);
    TYPE_IS(term_abt,
            env.GetFunctionType(annotations, env.GetIntType(annotations), {}));
  }
  SECTION(term_abu) {
    env.BindVariable(
        std::string_view{"f"},
        env.GetFunctionType(annotations, env.GetIntType(annotations), {}),
        nullptr);
    TYPE_IS(term_abu, env.GetIntType(annotations));
  }
  SECTION(term_abv) {
    env.BindVariable(std::string_view{"f"},
                     env.GetFunctionType(annotations,
                                         env.GetIntType(annotations),
                                         {env.GetIntType(annotations)}),
                     nullptr);
    TYPE_IS(term_abv, env.GetIntType(annotations));
  }
  SECTION(term_abw) {
    env.BindVariable(std::string_view{"f"},
                     env.GetFunctionType(annotations,
                                         env.GetIntType(annotations),
                                         {env.GetIntType(annotations),
                                          env.GetIntType(annotations)}),
                     nullptr);
    TYPE_IS(term_abw, env.GetIntType(annotations));
  }
  SECTION(term_abx) {
    env.BindVariable(std::string_view{"f"},
                     env.GetFunctionType(annotations,
                                         env.GetIntType(annotations),
                                         {env.GetIntType(annotations),
                                          env.GetIntType(annotations)}),
                     nullptr);
    env.BindVariable(std::string_view{"g"},
                     env.GetFunctionType(annotations,
                                         env.GetIntType(annotations),
                                         {env.GetIntType(annotations),
                                          env.GetIntType(annotations)}),
                     nullptr);
    TYPE_IS(term_abx, env.GetIntType(annotations));
  }
  SECTION(term_aby) {
    auto fn = env.GetFunctionType(annotations, env.GetIntType(annotations), {});
    env.BindVariable(std::string_view{"tuple"},
                     env.GetTupleType(annotations, {fn, fn}),
                     nullptr);
    TYPE_IS(term_aby, env.GetIntType(annotations));
  }
  SECTION(term_abz) {
    env.BindVariable(std::string_view{"x"},
                     env.GetIntType(annotations),
                     nullptr);
    auto fn = env.GetFunctionType(annotations, env.GetIntType(annotations), {});
    env.BindVariable(std::string_view{"array"},
                     env.GetArrayType(annotations, 5, fn),
                     nullptr);
    TYPE_IS(term_abz, env.GetIntType(annotations));
  }

  SECTION(term_aca) {
    TYPE_IS(term_aca,
            env.GetArrayType(annotations, 5, env.GetIntType(annotations)));
  }
  SECTION(term_acb) {
    TYPE_IS(term_acb,
            env.GetArrayType(
                annotations,
                3,
                env.GetArrayType(annotations, 2, env.GetIntType(annotations))));
  }
  SECTION(term_acc) {
    TYPE_IS(term_acc,
            env.GetTupleType(
                annotations,
                {env.GetIntType(annotations), env.GetIntType(annotations)}));
  }
  SECTION(term_acd) {
    auto point = env.GetTupleType(
        annotations,
        {env.GetIntType(annotations), env.GetIntType(annotations)});
    TYPE_IS(term_acd, env.GetTupleType(annotations, {point, point}));
  }
  SECTION(term_ace) {
    auto point = env.GetTupleType(
        annotations,
        {env.GetIntType(annotations), env.GetIntType(annotations)});
    TYPE_IS(term_ace, env.GetArrayType(annotations, 3, point));
  }
  SECTION(term_acf) {
    auto array = env.GetArrayType(annotations, 3, env.GetIntType(annotations));
    TYPE_IS(term_acf, env.GetTupleType(annotations, {array, array}));
  }
}

// NOLINTNEXTLINE
TEST_CASE("ast/action/Typecheck::Function", "[unit][ast][ast/action]") {
  pink::Type::Annotations annotations;
  annotations.IsInMemory(true);
  auto env = pink::CompilationUnit::CreateTestCompilationUnit();

  SECTION(term_acg) {
    TYPE_IS(term_acg,
            env.GetFunctionType(annotations, env.GetIntType(annotations), {}));
  }
  SECTION(term_ach) {
    TYPE_IS(term_ach,
            env.GetFunctionType(annotations, env.GetBoolType(annotations), {}));
  }
  SECTION(term_aci) {
    TYPE_IS(term_aci,
            env.GetFunctionType(annotations, env.GetBoolType(annotations), {}));
  }
  SECTION(term_acj) {
    TYPE_IS(term_acj,
            env.GetFunctionType(annotations, env.GetNilType(annotations), {}));
  }
  SECTION(term_ack) {
    TYPE_IS(term_ack,
            env.GetFunctionType(annotations, env.GetIntType(annotations), {}));
  }
  SECTION(term_acl) {
    TYPE_IS(term_acl,
            env.GetFunctionType(annotations, env.GetBoolType(annotations), {}));
  }
  SECTION(term_acm) {
    env.BindVariable(std::string_view{"a"},
                     env.GetIntType(annotations),
                     nullptr);
    TYPE_IS(term_acm,
            env.GetFunctionType(
                annotations,
                env.GetPointerType(annotations, env.GetIntType(annotations)),
                {}));
  }
  SECTION(term_acn) {
    env.BindVariable(
        std::string_view{"a"},
        env.GetPointerType(annotations, env.GetIntType(annotations)),
        nullptr);
    TYPE_IS(term_acn,
            env.GetFunctionType(annotations, env.GetIntType(annotations), {}));
  }

  SECTION(term_aco) {
    TYPE_IS(term_aco,
            env.GetFunctionType(annotations, env.GetIntType(annotations), {}));
  }
  SECTION(term_acp) {
    TYPE_IS(term_acp,
            env.GetFunctionType(annotations, env.GetIntType(annotations), {}));
  }
  SECTION(term_acq) {
    TYPE_IS(term_acq,
            env.GetFunctionType(annotations, env.GetIntType(annotations), {}));
  }
  SECTION(term_acr) {
    TYPE_IS(term_acr,
            env.GetFunctionType(annotations, env.GetIntType(annotations), {}));
  }
  SECTION(term_acs) {
    TYPE_IS(term_acs,
            env.GetFunctionType(annotations, env.GetIntType(annotations), {}));
  }
  SECTION(term_act) {
    TYPE_IS(term_act,
            env.GetFunctionType(annotations, env.GetBoolType(annotations), {}));
  }
  SECTION(term_acu) {
    TYPE_IS(term_acu,
            env.GetFunctionType(annotations, env.GetBoolType(annotations), {}));
  }
  SECTION(term_acv) {
    TYPE_IS(term_acv,
            env.GetFunctionType(annotations, env.GetBoolType(annotations), {}));
  }
  SECTION(term_acw) {
    TYPE_IS(term_acw,
            env.GetFunctionType(annotations, env.GetBoolType(annotations), {}));
  }
  SECTION(term_acx) {
    TYPE_IS(term_acx,
            env.GetFunctionType(annotations, env.GetBoolType(annotations), {}));
  }
  SECTION(term_acy) {
    TYPE_IS(term_acy,
            env.GetFunctionType(annotations, env.GetBoolType(annotations), {}));
  }
  SECTION(term_acz) {
    TYPE_IS(term_acz,
            env.GetFunctionType(annotations, env.GetBoolType(annotations), {}));
  }
  SECTION(term_ada) {
    TYPE_IS(term_ada,
            env.GetFunctionType(annotations, env.GetBoolType(annotations), {}));
  }
  SECTION(term_adb) {
    TYPE_IS(term_adb,
            env.GetFunctionType(annotations, env.GetBoolType(annotations), {}));
  }
  SECTION(term_adc) {
    TYPE_IS(term_adc,
            env.GetFunctionType(annotations, env.GetBoolType(annotations), {}));
  }

  SECTION(term_add) {
    env.BindVariable(std::string_view{"a"},
                     env.GetIntType(annotations),
                     nullptr);
    TYPE_IS(term_add,
            env.GetFunctionType(annotations, env.GetIntType(annotations), {}));
  }
  SECTION(term_ade) {
    env.BindVariable(std::string_view{"a"},
                     env.GetIntType(annotations),
                     nullptr);
    TYPE_IS(term_ade,
            env.GetFunctionType(annotations, env.GetIntType(annotations), {}));
  }
  SECTION(term_adf) {
    auto array = env.GetArrayType(annotations, 5, env.GetIntType(annotations));
    TYPE_IS(term_adf, env.GetFunctionType(annotations, array, {}));
  }
  SECTION(term_adg) {
    auto array = env.GetArrayType(
        annotations,
        3,
        env.GetArrayType(annotations, 2, env.GetIntType(annotations)));
    TYPE_IS(term_adg, env.GetFunctionType(annotations, array, {}));
  }
  SECTION(term_adh) {
    auto point = env.GetTupleType(
        annotations,
        {env.GetIntType(annotations), env.GetIntType(annotations)});
    TYPE_IS(term_adh, env.GetFunctionType(annotations, point, {}));
  }
  SECTION(term_adi) {
    auto point = env.GetTupleType(
        annotations,
        {env.GetIntType(annotations), env.GetIntType(annotations)});
    auto pair = env.GetTupleType(annotations, {point, point});
    TYPE_IS(term_adi, env.GetFunctionType(annotations, pair, {}));
  }
  SECTION(term_adj) {
    auto point = env.GetTupleType(
        annotations,
        {env.GetIntType(annotations), env.GetIntType(annotations)});
    auto array = env.GetArrayType(annotations, 3, point);
    TYPE_IS(term_adj, env.GetFunctionType(annotations, array, {}));
  }
  SECTION(term_adk) {
    auto array = env.GetArrayType(annotations, 3, env.GetIntType(annotations));
    auto pair  = env.GetTupleType(annotations, {array, array});
    TYPE_IS(term_adk, env.GetFunctionType(annotations, pair, {}));
  }

  SECTION(term_adl) {
    env.BindVariable(std::string_view{"x"},
                     env.GetIntType(annotations),
                     nullptr);
    env.BindVariable(
        std::string_view{"array"},
        env.GetArrayType(annotations, 5, env.GetIntType(annotations)),
        nullptr);
    TYPE_IS(term_adl,
            env.GetFunctionType(annotations, env.GetIntType(annotations), {}));
  }
  SECTION(term_adm) {
    env.BindVariable(std::string_view{"x"},
                     env.GetIntType(annotations),
                     nullptr);
    env.BindVariable(std::string_view{"y"},
                     env.GetIntType(annotations),
                     nullptr);
    env.BindVariable(
        std::string_view{"array"},
        env.GetArrayType(
            annotations,
            5,
            env.GetArrayType(annotations, 5, env.GetIntType(annotations))),
        nullptr);
    TYPE_IS(term_adm,
            env.GetFunctionType(annotations, env.GetIntType(annotations), {}));
  }
  SECTION(term_adn) {
    env.BindVariable(std::string_view{"tuple"},
                     env.GetTupleType(annotations,
                                      {env.GetIntType(annotations),
                                       env.GetIntType(annotations)}),
                     nullptr);
    TYPE_IS(term_adn,
            env.GetFunctionType(annotations, env.GetIntType(annotations), {}));
  }
  SECTION(term_ado) {
    auto point = env.GetTupleType(
        annotations,
        {env.GetIntType(annotations), env.GetIntType(annotations)});
    auto pair = env.GetTupleType(annotations, {point, point});
    env.BindVariable(std::string_view{"tuple"}, pair, nullptr);
    TYPE_IS(term_ado,
            env.GetFunctionType(annotations, env.GetIntType(annotations), {}));
  }
  SECTION(term_adp) {
    env.BindVariable(std::string_view{"x"},
                     env.GetIntType(annotations),
                     nullptr);
    auto point = env.GetTupleType(
        annotations,
        {env.GetIntType(annotations), env.GetIntType(annotations)});
    auto array = env.GetArrayType(annotations, 5, point);
    env.BindVariable(std::string_view{"array"}, array, nullptr);
    TYPE_IS(term_adp,
            env.GetFunctionType(annotations, env.GetIntType(annotations), {}));
  }
  SECTION(term_adq) {
    env.BindVariable(std::string_view{"x"},
                     env.GetIntType(annotations),
                     nullptr);
    auto array = env.GetArrayType(annotations, 5, env.GetIntType(annotations));
    auto pair  = env.GetTupleType(annotations, {array, array});
    env.BindVariable(std::string_view{"tuple"}, pair, nullptr);
    TYPE_IS(term_adq,
            env.GetFunctionType(annotations, env.GetIntType(annotations), {}));
  }
  SECTION(term_adr) {
    env.BindVariable(std::string_view{"x"},
                     env.GetIntType(annotations),
                     nullptr);
    env.BindVariable(
        std::string_view{"array"},
        env.GetArrayType(annotations, 5, env.GetIntType(annotations)),
        nullptr);
    TYPE_IS(term_adr,
            env.GetFunctionType(annotations, env.GetIntType(annotations), {}));
  }
  SECTION(term_ads) {
    env.BindVariable(std::string_view{"tuple"},
                     env.GetTupleType(annotations,
                                      {env.GetBoolType(annotations),
                                       env.GetBoolType(annotations)}),
                     nullptr);
    TYPE_IS(term_ads,
            env.GetFunctionType(annotations, env.GetBoolType(annotations), {}));
  }

  SECTION(term_adt) {
    auto fn = env.GetFunctionType(annotations, env.GetIntType(annotations), {});
    env.BindVariable(std::string_view{"f"}, fn, nullptr);
    TYPE_IS(term_adt, env.GetFunctionType(annotations, fn, {}));
  }
  SECTION(term_adu) {
    auto fn = env.GetFunctionType(annotations, env.GetIntType(annotations), {});
    env.BindVariable(std::string_view{"f"}, fn, nullptr);
    TYPE_IS(term_adu,
            env.GetFunctionType(annotations, env.GetIntType(annotations), {}));
  }
  SECTION(term_adv) {
    auto fn = env.GetFunctionType(annotations,
                                  env.GetIntType(annotations),
                                  {env.GetIntType(annotations)});
    env.BindVariable(std::string_view{"f"}, fn, nullptr);
    TYPE_IS(term_adv,
            env.GetFunctionType(annotations, env.GetIntType(annotations), {}));
  }
  SECTION(term_adw) {
    auto fn = env.GetFunctionType(
        annotations,
        env.GetIntType(annotations),
        {env.GetIntType(annotations), env.GetIntType(annotations)});
    env.BindVariable(std::string_view{"f"}, fn, nullptr);
    TYPE_IS(term_adw,
            env.GetFunctionType(annotations, env.GetIntType(annotations), {}));
  }
  SECTION(term_adx) {
    auto fn = env.GetFunctionType(
        annotations,
        env.GetIntType(annotations),
        {env.GetIntType(annotations), env.GetIntType(annotations)});
    env.BindVariable(std::string_view{"f"}, fn, nullptr);
    env.BindVariable(std::string_view{"g"}, fn, nullptr);
    TYPE_IS(term_adx,
            env.GetFunctionType(annotations, env.GetIntType(annotations), {}));
  }
  SECTION(term_ady) {
    env.BindVariable(std::string_view{"x"},
                     env.GetIntType(annotations),
                     nullptr);
    auto fn = env.GetFunctionType(annotations, env.GetIntType(annotations), {});
    env.BindVariable(std::string_view{"array"},
                     env.GetArrayType(annotations, 5, fn),
                     nullptr);
    TYPE_IS(term_ady,
            env.GetFunctionType(annotations, env.GetIntType(annotations), {}));
  }
  SECTION(term_adz) {
    auto fn = env.GetFunctionType(annotations, env.GetIntType(annotations), {});
    env.BindVariable(std::string_view{"tuple"},
                     env.GetTupleType(annotations, {fn, fn}),
                     nullptr);
    TYPE_IS(term_adz,
            env.GetFunctionType(annotations, env.GetIntType(annotations), {}));
  }

  SECTION(term_aea) {
    TYPE_IS(term_aea,
            env.GetFunctionType(annotations,
                                env.GetIntType(annotations),
                                {env.GetIntType(annotations)}));
  }
  SECTION(term_aeb) {
    TYPE_IS(term_aeb,
            env.GetFunctionType(annotations,
                                env.GetBoolType(annotations),
                                {env.GetBoolType(annotations)}));
  }
  SECTION(term_aec) {
    TYPE_IS(term_aec,
            env.GetFunctionType(annotations,
                                env.GetNilType(annotations),
                                {env.GetNilType(annotations)}));
  }
  SECTION(term_aed) {
    auto pointer = env.GetPointerType(annotations, env.GetIntType(annotations));
    TYPE_IS(term_aed, env.GetFunctionType(annotations, pointer, {pointer}));
  }
  SECTION(term_aee) {
    auto slice = env.GetSliceType(annotations, env.GetIntType(annotations));
    TYPE_IS(term_aee, env.GetFunctionType(annotations, slice, {slice}));
  }
  SECTION(term_aef) {
    auto point = env.GetTupleType(
        annotations,
        {env.GetIntType(annotations), env.GetIntType(annotations)});
    TYPE_IS(term_aef, env.GetFunctionType(annotations, point, {point}));
  }
  SECTION(term_aeg) {
    auto array = env.GetArrayType(annotations, 10, env.GetIntType(annotations));
    TYPE_IS(term_aeg, env.GetFunctionType(annotations, array, {array}));
  }
  SECTION(term_aeh) {
    auto pointer = env.GetPointerType(
        annotations,
        env.GetPointerType(annotations, env.GetIntType(annotations)));
    TYPE_IS(term_aeh, env.GetFunctionType(annotations, pointer, {pointer}));
  }
  SECTION(term_aei) {
    auto pointer = env.GetPointerType(
        annotations,
        env.GetSliceType(annotations, env.GetIntType(annotations)));
    TYPE_IS(term_aei, env.GetFunctionType(annotations, pointer, {pointer}));
  }
  SECTION(term_aej) {
    auto slice = env.GetSliceType(
        annotations,
        env.GetSliceType(annotations, env.GetIntType(annotations)));
    TYPE_IS(term_aej, env.GetFunctionType(annotations, slice, {slice}));
  }
  SECTION(term_aek) {
    auto slice = env.GetSliceType(
        annotations,
        env.GetPointerType(annotations, env.GetIntType(annotations)));
    TYPE_IS(term_aek, env.GetFunctionType(annotations, slice, {slice}));
  }
  SECTION(term_ael) {
    auto fn = env.GetFunctionType(annotations, env.GetIntType(annotations), {});
    TYPE_IS(term_ael, env.GetFunctionType(annotations, fn, {fn}));
  }

  SECTION(term_aem) {
    TYPE_IS(term_aem,
            env.GetFunctionType(
                annotations,
                env.GetIntType(annotations),
                {env.GetIntType(annotations), env.GetIntType(annotations)}));
  }
  SECTION(term_aen) {
    TYPE_IS(term_aen,
            env.GetFunctionType(
                annotations,
                env.GetBoolType(annotations),
                {env.GetBoolType(annotations), env.GetBoolType(annotations)}));
  }
  SECTION(term_aeo) {
    TYPE_IS(term_aeo,
            env.GetFunctionType(
                annotations,
                env.GetIntType(annotations),
                {env.GetPointerType(annotations, env.GetIntType(annotations)),
                 env.GetIntType(annotations)}));
  }
  SECTION(term_aep) {
    TYPE_IS(term_aep,
            env.GetFunctionType(
                annotations,
                env.GetIntType(annotations),
                {env.GetSliceType(annotations, env.GetIntType(annotations)),
                 env.GetIntType(annotations),
                 env.GetIntType(annotations)}));
  }
  SECTION(term_aeq) {
    TYPE_IS(
        term_aeq,
        env.GetFunctionType(annotations,
                            env.GetIntType(annotations),
                            {env.GetTupleType(annotations,
                                              {env.GetIntType(annotations),
                                               env.GetIntType(annotations)})}));
  }
  SECTION(term_aer) {
    TYPE_IS(term_aer,
            env.GetFunctionType(
                annotations,
                env.GetIntType(annotations),
                {env.GetArrayType(annotations, 10, env.GetIntType(annotations)),
                 env.GetIntType(annotations),
                 env.GetIntType(annotations)}));
  }
  SECTION(term_aes) {
    TYPE_IS(
        term_aes,
        env.GetFunctionType(
            annotations,
            env.GetIntType(annotations),
            {env.GetPointerType(
                 annotations,
                 env.GetPointerType(annotations, env.GetIntType(annotations))),
             env.GetIntType(annotations)}));
  }
  SECTION(term_aet) {
    TYPE_IS(
        term_aet,
        env.GetFunctionType(
            annotations,
            env.GetIntType(annotations),
            {env.GetPointerType(
                 annotations,
                 env.GetSliceType(annotations, env.GetIntType(annotations))),
             env.GetIntType(annotations),
             env.GetIntType(annotations)}));
  }
  SECTION(term_aeu) {
    TYPE_IS(
        term_aeu,
        env.GetFunctionType(
            annotations,
            env.GetIntType(annotations),
            {env.GetSliceType(
                 annotations,
                 env.GetSliceType(annotations, env.GetIntType(annotations))),
             env.GetIntType(annotations),
             env.GetIntType(annotations)}));
  }
  SECTION(term_aev) {
    TYPE_IS(
        term_aev,
        env.GetFunctionType(
            annotations,
            env.GetIntType(annotations),
            {env.GetSliceType(
                 annotations,
                 env.GetPointerType(annotations, env.GetIntType(annotations))),
             env.GetIntType(annotations),
             env.GetIntType(annotations)}));
  }
  SECTION(term_aew) {
    TYPE_IS(
        term_aew,
        env.GetFunctionType(annotations,
                            env.GetIntType(annotations),
                            {env.GetFunctionType(annotations,
                                                 env.GetIntType(annotations),
                                                 {})}));
  }
  SECTION(term_aex) {
    TYPE_IS(
        term_aex,
        env.GetFunctionType(annotations,
                            env.GetIntType(annotations),
                            {env.GetFunctionType(annotations,
                                                 env.GetIntType(annotations),
                                                 {env.GetIntType(annotations),
                                                  env.GetIntType(annotations)}),
                             env.GetIntType(annotations),
                             env.GetIntType(annotations)}));
  }
}