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
  auto env = pink::CompilationUnit::CreateTestCompilationUnit();

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
  SECTION(term_aby) {
    auto fn = env.GetFunctionType(env.GetIntType(), {});
    env.BindVariable(std::string_view{"tuple"},
                     env.GetTupleType({fn, fn}),
                     nullptr);
    TYPE_IS(term_aby, env.GetIntType());
  }
  SECTION(term_abz) {
    env.BindVariable(std::string_view{"x"}, env.GetIntType(), nullptr);
    auto fn = env.GetFunctionType(env.GetIntType(), {});
    env.BindVariable(std::string_view{"array"},
                     env.GetArrayType(5, fn),
                     nullptr);
    TYPE_IS(term_abz, env.GetIntType());
  }

  SECTION(term_aca) {
    TYPE_IS(term_aca, env.GetArrayType(5, env.GetIntType()));
  }
  SECTION(term_acb) {
    TYPE_IS(term_acb,
            env.GetArrayType(3, env.GetArrayType(2, env.GetIntType())));
  }
  SECTION(term_acc) {
    TYPE_IS(term_acc, env.GetTupleType({env.GetIntType(), env.GetIntType()}));
  }
  SECTION(term_acd) {
    auto point = env.GetTupleType({env.GetIntType(), env.GetIntType()});
    TYPE_IS(term_acd, env.GetTupleType({point, point}));
  }
  SECTION(term_ace) {
    auto point = env.GetTupleType({env.GetIntType(), env.GetIntType()});
    TYPE_IS(term_ace, env.GetArrayType(3, point));
  }
  SECTION(term_acf) {
    auto array = env.GetArrayType(3, env.GetIntType());
    TYPE_IS(term_acf, env.GetTupleType({array, array}));
  }
}

// NOLINTNEXTLINE
TEST_CASE("ast/action/Typecheck::Function", "[unit][ast][ast/action]") {
  auto env = pink::CompilationUnit::CreateTestCompilationUnit();

  SECTION(term_acg) {
    TYPE_IS(term_acg, env.GetFunctionType(env.GetIntType(), {}));
  }
  SECTION(term_ach) {
    TYPE_IS(term_ach, env.GetFunctionType(env.GetBoolType(), {}));
  }
  SECTION(term_aci) {
    TYPE_IS(term_aci, env.GetFunctionType(env.GetBoolType(), {}));
  }
  SECTION(term_acj) {
    TYPE_IS(term_acj, env.GetFunctionType(env.GetNilType(), {}));
  }
  SECTION(term_ack) {
    TYPE_IS(term_ack, env.GetFunctionType(env.GetIntType(), {}));
  }
  SECTION(term_acl) {
    TYPE_IS(term_acl, env.GetFunctionType(env.GetBoolType(), {}));
  }
  SECTION(term_acm) {
    env.BindVariable(std::string_view{"a"}, env.GetIntType(), nullptr);
    TYPE_IS(term_acm,
            env.GetFunctionType(env.GetPointerType(env.GetIntType()), {}));
  }
  SECTION(term_acn) {
    env.BindVariable(std::string_view{"a"},
                     env.GetPointerType(env.GetIntType()),
                     nullptr);
    TYPE_IS(term_acn, env.GetFunctionType(env.GetIntType(), {}));
  }

  SECTION(term_aco) {
    TYPE_IS(term_aco, env.GetFunctionType(env.GetIntType(), {}));
  }
  SECTION(term_acp) {
    TYPE_IS(term_acp, env.GetFunctionType(env.GetIntType(), {}));
  }
  SECTION(term_acq) {
    TYPE_IS(term_acq, env.GetFunctionType(env.GetIntType(), {}));
  }
  SECTION(term_acr) {
    TYPE_IS(term_acr, env.GetFunctionType(env.GetIntType(), {}));
  }
  SECTION(term_acs) {
    TYPE_IS(term_acs, env.GetFunctionType(env.GetIntType(), {}));
  }
  SECTION(term_act) {
    TYPE_IS(term_act, env.GetFunctionType(env.GetBoolType(), {}));
  }
  SECTION(term_acu) {
    TYPE_IS(term_acu, env.GetFunctionType(env.GetBoolType(), {}));
  }
  SECTION(term_acv) {
    TYPE_IS(term_acv, env.GetFunctionType(env.GetBoolType(), {}));
  }
  SECTION(term_acw) {
    TYPE_IS(term_acw, env.GetFunctionType(env.GetBoolType(), {}));
  }
  SECTION(term_acx) {
    TYPE_IS(term_acx, env.GetFunctionType(env.GetBoolType(), {}));
  }
  SECTION(term_acy) {
    TYPE_IS(term_acy, env.GetFunctionType(env.GetBoolType(), {}));
  }
  SECTION(term_acz) {
    TYPE_IS(term_acz, env.GetFunctionType(env.GetBoolType(), {}));
  }
  SECTION(term_ada) {
    TYPE_IS(term_ada, env.GetFunctionType(env.GetBoolType(), {}));
  }
  SECTION(term_adb) {
    TYPE_IS(term_adb, env.GetFunctionType(env.GetBoolType(), {}));
  }
  SECTION(term_adc) {
    TYPE_IS(term_adc, env.GetFunctionType(env.GetBoolType(), {}));
  }

  SECTION(term_add) {
    env.BindVariable(std::string_view{"a"}, env.GetIntType(), nullptr);
    TYPE_IS(term_add, env.GetFunctionType(env.GetIntType(), {}));
  }
  SECTION(term_ade) {
    env.BindVariable(std::string_view{"a"}, env.GetIntType(), nullptr);
    TYPE_IS(term_ade, env.GetFunctionType(env.GetIntType(), {}));
  }
  SECTION(term_adf) {
    auto array = env.GetArrayType(5, env.GetIntType());
    TYPE_IS(term_adf, env.GetFunctionType(array, {}));
  }
  SECTION(term_adg) {
    auto array = env.GetArrayType(3, env.GetArrayType(2, env.GetIntType()));
    TYPE_IS(term_adg, env.GetFunctionType(array, {}));
  }
  SECTION(term_adh) {
    auto point = env.GetTupleType({env.GetIntType(), env.GetIntType()});
    TYPE_IS(term_adh, env.GetFunctionType(point, {}));
  }
  SECTION(term_adi) {
    auto point = env.GetTupleType({env.GetIntType(), env.GetIntType()});
    auto pair  = env.GetTupleType({point, point});
    TYPE_IS(term_adi, env.GetFunctionType(pair, {}));
  }
  SECTION(term_adj) {
    auto point = env.GetTupleType({env.GetIntType(), env.GetIntType()});
    auto array = env.GetArrayType(3, point);
    TYPE_IS(term_adj, env.GetFunctionType(array, {}));
  }
  SECTION(term_adk) {
    auto array = env.GetArrayType(3, env.GetIntType());
    auto pair  = env.GetTupleType({array, array});
    TYPE_IS(term_adk, env.GetFunctionType(pair, {}));
  }

  SECTION(term_adl) {
    env.BindVariable(std::string_view{"x"}, env.GetIntType(), nullptr);
    env.BindVariable(std::string_view{"array"},
                     env.GetArrayType(5, env.GetIntType()),
                     nullptr);
    TYPE_IS(term_adl, env.GetFunctionType(env.GetIntType(), {}));
  }
  SECTION(term_adm) {
    env.BindVariable(std::string_view{"x"}, env.GetIntType(), nullptr);
    env.BindVariable(std::string_view{"y"}, env.GetIntType(), nullptr);
    env.BindVariable(std::string_view{"array"},
                     env.GetArrayType(5, env.GetArrayType(5, env.GetIntType())),
                     nullptr);
    TYPE_IS(term_adm, env.GetFunctionType(env.GetIntType(), {}));
  }
  SECTION(term_adn) {
    env.BindVariable(std::string_view{"tuple"},
                     env.GetTupleType({env.GetIntType(), env.GetIntType()}),
                     nullptr);
    TYPE_IS(term_adn, env.GetFunctionType(env.GetIntType(), {}));
  }
  SECTION(term_ado) {
    auto point = env.GetTupleType({env.GetIntType(), env.GetIntType()});
    auto pair  = env.GetTupleType({point, point});
    env.BindVariable(std::string_view{"tuple"}, pair, nullptr);
    TYPE_IS(term_ado, env.GetFunctionType(env.GetIntType(), {}));
  }
  SECTION(term_adp) {
    env.BindVariable(std::string_view{"x"}, env.GetIntType(), nullptr);
    auto point = env.GetTupleType({env.GetIntType(), env.GetIntType()});
    auto array = env.GetArrayType(5, point);
    env.BindVariable(std::string_view{"array"}, array, nullptr);
    TYPE_IS(term_adp, env.GetFunctionType(env.GetIntType(), {}));
  }
  SECTION(term_adq) {
    env.BindVariable(std::string_view{"x"}, env.GetIntType(), nullptr);
    auto array = env.GetArrayType(5, env.GetIntType());
    auto pair  = env.GetTupleType({array, array});
    env.BindVariable(std::string_view{"tuple"}, pair, nullptr);
    TYPE_IS(term_adq, env.GetFunctionType(env.GetIntType(), {}));
  }
  SECTION(term_adr) {
    env.BindVariable(std::string_view{"x"}, env.GetIntType(), nullptr);
    env.BindVariable(std::string_view{"array"},
                     env.GetArrayType(5, env.GetIntType()),
                     nullptr);
    TYPE_IS(term_adr, env.GetFunctionType(env.GetIntType(), {}));
  }
  SECTION(term_ads) {
    env.BindVariable(std::string_view{"tuple"},
                     env.GetTupleType({env.GetBoolType(), env.GetBoolType()}),
                     nullptr);
    TYPE_IS(term_ads, env.GetFunctionType(env.GetBoolType(), {}));
  }

  SECTION(term_adt) {
    auto fn = env.GetFunctionType(env.GetIntType(), {});
    env.BindVariable(std::string_view{"f"}, fn, nullptr);
    TYPE_IS(term_adt, env.GetFunctionType(fn, {}));
  }
  SECTION(term_adu) {
    auto fn = env.GetFunctionType(env.GetIntType(), {});
    env.BindVariable(std::string_view{"f"}, fn, nullptr);
    TYPE_IS(term_adu, env.GetFunctionType(env.GetIntType(), {}));
  }
  SECTION(term_adv) {
    auto fn = env.GetFunctionType(env.GetIntType(), {env.GetIntType()});
    env.BindVariable(std::string_view{"f"}, fn, nullptr);
    TYPE_IS(term_adv, env.GetFunctionType(env.GetIntType(), {}));
  }
  SECTION(term_adw) {
    auto fn = env.GetFunctionType(env.GetIntType(),
                                  {env.GetIntType(), env.GetIntType()});
    env.BindVariable(std::string_view{"f"}, fn, nullptr);
    TYPE_IS(term_adw, env.GetFunctionType(env.GetIntType(), {}));
  }
  SECTION(term_adx) {
    auto fn = env.GetFunctionType(env.GetIntType(),
                                  {env.GetIntType(), env.GetIntType()});
    env.BindVariable(std::string_view{"f"}, fn, nullptr);
    env.BindVariable(std::string_view{"g"}, fn, nullptr);
    TYPE_IS(term_adx, env.GetFunctionType(env.GetIntType(), {}));
  }
  SECTION(term_ady) {
    env.BindVariable(std::string_view{"x"}, env.GetIntType(), nullptr);
    auto fn = env.GetFunctionType(env.GetIntType(), {});
    env.BindVariable(std::string_view{"array"},
                     env.GetArrayType(5, fn),
                     nullptr);
    TYPE_IS(term_ady, env.GetFunctionType(env.GetIntType(), {}));
  }
  SECTION(term_adz) {
    auto fn = env.GetFunctionType(env.GetIntType(), {});
    env.BindVariable(std::string_view{"tuple"},
                     env.GetTupleType({fn, fn}),
                     nullptr);
    TYPE_IS(term_adz, env.GetFunctionType(env.GetIntType(), {}));
  }

  SECTION(term_aea) {
    TYPE_IS(term_aea,
            env.GetFunctionType(env.GetIntType(), {env.GetIntType()}));
  }
  SECTION(term_aeb) {
    TYPE_IS(term_aeb,
            env.GetFunctionType(env.GetBoolType(), {env.GetBoolType()}));
  }
  SECTION(term_aec) {
    TYPE_IS(term_aec,
            env.GetFunctionType(env.GetNilType(), {env.GetNilType()}));
  }
  SECTION(term_aed) {
    auto pointer = env.GetPointerType(env.GetIntType());
    TYPE_IS(term_aed, env.GetFunctionType(pointer, {pointer}));
  }
  SECTION(term_aee) {
    auto slice = env.GetSliceType(env.GetIntType());
    TYPE_IS(term_aee, env.GetFunctionType(slice, {slice}));
  }
  SECTION(term_aef) {
    auto point = env.GetTupleType({env.GetIntType(), env.GetIntType()});
    TYPE_IS(term_aef, env.GetFunctionType(point, {point}));
  }
  SECTION(term_aeg) {
    auto array = env.GetArrayType(10, env.GetIntType());
    TYPE_IS(term_aeg, env.GetFunctionType(array, {array}));
  }
  SECTION(term_aeh) {
    auto pointer = env.GetPointerType(env.GetPointerType(env.GetIntType()));
    TYPE_IS(term_aeh, env.GetFunctionType(pointer, {pointer}));
  }
  SECTION(term_aei) {
    auto pointer = env.GetPointerType(env.GetSliceType(env.GetIntType()));
    TYPE_IS(term_aei, env.GetFunctionType(pointer, {pointer}));
  }
  SECTION(term_aej) {
    auto slice = env.GetSliceType(env.GetSliceType(env.GetIntType()));
    TYPE_IS(term_aej, env.GetFunctionType(slice, {slice}));
  }
  SECTION(term_aek) {
    auto slice = env.GetSliceType(env.GetPointerType(env.GetIntType()));
    TYPE_IS(term_aek, env.GetFunctionType(slice, {slice}));
  }
  SECTION(term_ael) {
    auto fn = env.GetFunctionType(env.GetIntType(), {});
    TYPE_IS(term_ael, env.GetFunctionType(fn, {fn}));
  }

  SECTION(term_aem) {
    TYPE_IS(term_aem,
            env.GetFunctionType(env.GetIntType(),
                                {env.GetIntType(), env.GetIntType()}));
  }
  SECTION(term_aen) {
    TYPE_IS(term_aen,
            env.GetFunctionType(env.GetBoolType(),
                                {env.GetBoolType(), env.GetBoolType()}));
  }
  SECTION(term_aeo) {
    TYPE_IS(term_aeo,
            env.GetFunctionType(
                env.GetIntType(),
                {env.GetPointerType(env.GetIntType()), env.GetIntType()}));
  }
  SECTION(term_aep) {
    TYPE_IS(term_aep,
            env.GetFunctionType(env.GetIntType(),
                                {env.GetSliceType(env.GetIntType()),
                                 env.GetIntType(),
                                 env.GetIntType()}));
  }
  SECTION(term_aeq) {
    TYPE_IS(term_aeq,
            env.GetFunctionType(
                env.GetIntType(),
                {env.GetTupleType({env.GetIntType(), env.GetIntType()})}));
  }
  SECTION(term_aer) {
    TYPE_IS(term_aer,
            env.GetFunctionType(env.GetIntType(),
                                {env.GetArrayType(10, env.GetIntType()),
                                 env.GetIntType(),
                                 env.GetIntType()}));
  }
  SECTION(term_aes) {
    TYPE_IS(term_aes,
            env.GetFunctionType(
                env.GetIntType(),
                {env.GetPointerType(env.GetPointerType(env.GetIntType())),
                 env.GetIntType()}));
  }
  SECTION(term_aet) {
    TYPE_IS(term_aet,
            env.GetFunctionType(
                env.GetIntType(),
                {env.GetPointerType(env.GetSliceType(env.GetIntType())),
                 env.GetIntType(),
                 env.GetIntType()}));
  }
  SECTION(term_aeu) {
    TYPE_IS(term_aeu,
            env.GetFunctionType(
                env.GetIntType(),
                {env.GetSliceType(env.GetSliceType(env.GetIntType())),
                 env.GetIntType(),
                 env.GetIntType()}));
  }
  SECTION(term_aev) {
    TYPE_IS(term_aev,
            env.GetFunctionType(
                env.GetIntType(),
                {env.GetSliceType(env.GetPointerType(env.GetIntType())),
                 env.GetIntType(),
                 env.GetIntType()}));
  }
  SECTION(term_aew) {
    TYPE_IS(term_aew,
            env.GetFunctionType(env.GetIntType(),
                                {env.GetFunctionType(env.GetIntType(), {})}));
  }
  SECTION(term_aex) {
    TYPE_IS(term_aex,
            env.GetFunctionType(
                env.GetIntType(),
                {env.GetFunctionType(env.GetIntType(),
                                     {env.GetIntType(), env.GetIntType()}),
                 env.GetIntType(),
                 env.GetIntType()}));
  }
}