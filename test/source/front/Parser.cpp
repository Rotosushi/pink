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

#define PARSE(text)                                                            \
  std::stringstream stream{text};                                              \
  pink::Location    location{ComputeLocation(text)};                           \
  env.SetIStream(&stream);                                                     \
  auto parse_result = env.Parse();                                             \
  if (!parse_result) {                                                         \
    env.PrintErrorWithSourceText(std::cerr, parse_result.GetSecond());         \
    FAIL("Unable to parse expression.");                                       \
  }                                                                            \
  REQUIRE(parse_result);                                                       \
  auto *expression = parse_result.GetFirst().get();                            \
  CHECK(expression->GetLocation() == location)

// NOLINTNEXTLINE
TEST_CASE("front/Parser::ParseBind", "[unit][front]") {
  auto env = pink::CompilationUnit::CreateTestCompilationUnit();
  /*
    Parsing Bind Expressions
  */
  SECTION(term_aaa) { PARSE(term_aaa); }
  SECTION(term_aab) { PARSE(term_aab); }
  SECTION(term_aac) { PARSE(term_aac); }
  SECTION(term_aad) { PARSE(term_aad); }
  SECTION(term_aae) { PARSE(term_aae); }
  SECTION(term_aaf) { PARSE(term_aaf); }
  SECTION(term_aag) { PARSE(term_aaf); }
  SECTION(term_aah) { PARSE(term_aah); }
  SECTION(term_aai) { PARSE(term_aai); }

  SECTION(term_aaj) { PARSE(term_aaj); }
  SECTION(term_aak) { PARSE(term_aak); }
  SECTION(term_aal) { PARSE(term_aal); }
  SECTION(term_aam) { PARSE(term_aam); }
  SECTION(term_aan) { PARSE(term_aan); }
  SECTION(term_aao) { PARSE(term_aao); }
  SECTION(term_aap) { PARSE(term_aap); }
  SECTION(term_aaq) { PARSE(term_aaq); }
  SECTION(term_aar) { PARSE(term_aar); }
  SECTION(term_aas) { PARSE(term_aas); }
  SECTION(term_aat) { PARSE(term_aat); }
  SECTION(term_aau) { PARSE(term_aau); }
  SECTION(term_aav) { PARSE(term_aav); }
  SECTION(term_aaw) { PARSE(term_aaw); }
  SECTION(term_aax) { PARSE(term_aax); }

  SECTION(term_aay) { PARSE(term_aay); }
  SECTION(term_aaz) { PARSE(term_aaz); }
  SECTION(term_aba) { PARSE(term_aba); }
  SECTION(term_abb) { PARSE(term_abb); }
  SECTION(term_abc) { PARSE(term_abc); }
  SECTION(term_abd) { PARSE(term_abd); }

  SECTION(term_abe) { PARSE(term_abe); }
  SECTION(term_abf) { PARSE(term_abf); }
  SECTION(term_abg) { PARSE(term_abg); }
  SECTION(term_abh) { PARSE(term_abh); }
  SECTION(term_abi) { PARSE(term_abi); }
  SECTION(term_abj) { PARSE(term_abj); }
  SECTION(term_abk) { PARSE(term_abk); }
  SECTION(term_abl) { PARSE(term_abl); }
  SECTION(term_abm) { PARSE(term_abm); }
  SECTION(term_abn) { PARSE(term_abn); }
  SECTION(term_abo) { PARSE(term_abo); }
  SECTION(term_abp) { PARSE(term_abp); }
  SECTION(term_abq) { PARSE(term_abq); }
  SECTION(term_abr) { PARSE(term_abr); }

  SECTION(term_abt) { PARSE(term_abt); }
  SECTION(term_abu) { PARSE(term_abu); }
  SECTION(term_abv) { PARSE(term_abv); }
  SECTION(term_abw) { PARSE(term_abw); }
  SECTION(term_abx) { PARSE(term_abx); }
  SECTION(term_aby) { PARSE(term_aby); }
  SECTION(term_abz) { PARSE(term_abz); }

  SECTION(term_aca) { PARSE(term_aca); }
  SECTION(term_acb) { PARSE(term_acb); }
  SECTION(term_acc) { PARSE(term_acc); }
  SECTION(term_acd) { PARSE(term_acd); }
  SECTION(term_ace) { PARSE(term_ace); }
  SECTION(term_acf) { PARSE(term_acf); }
}

TEST_CASE("front/Parser::ParseFunction", "[unit][front]") {
  auto env = pink::CompilationUnit::CreateTestCompilationUnit();
  /*
    Parsing Function Definitions
  */
  SECTION(term_acg) { PARSE(term_acg); }
  SECTION(term_ach) { PARSE(term_ach); }
  SECTION(term_aci) { PARSE(term_aci); }
  SECTION(term_acj) { PARSE(term_acj); }
  SECTION(term_ack) { PARSE(term_ack); }
  SECTION(term_acl) { PARSE(term_acl); }
  SECTION(term_acm) { PARSE(term_acm); }
  SECTION(term_acn) { PARSE(term_acn); }

  SECTION(term_aco) { PARSE(term_aco); }
  SECTION(term_acp) { PARSE(term_acp); }
  SECTION(term_acq) { PARSE(term_acq); }
  SECTION(term_acr) { PARSE(term_acr); }
  SECTION(term_acs) { PARSE(term_acs); }
  SECTION(term_act) { PARSE(term_act); }
  SECTION(term_acu) { PARSE(term_acu); }
  SECTION(term_acv) { PARSE(term_acv); }
  SECTION(term_acw) { PARSE(term_acw); }
  SECTION(term_acx) { PARSE(term_acx); }
  SECTION(term_acy) { PARSE(term_acy); }
  SECTION(term_acz) { PARSE(term_acz); }
  SECTION(term_ada) { PARSE(term_ada); }
  SECTION(term_adb) { PARSE(term_adb); }
  SECTION(term_adc) { PARSE(term_adc); }

  SECTION(term_add) { PARSE(term_add); }
  SECTION(term_ade) { PARSE(term_ade); }
  SECTION(term_adf) { PARSE(term_adf); }
  SECTION(term_adg) { PARSE(term_adg); }
  SECTION(term_adh) { PARSE(term_adh); }
  SECTION(term_adi) { PARSE(term_adi); }
  SECTION(term_adj) { PARSE(term_adj); }
  SECTION(term_adk) { PARSE(term_adk); }

  SECTION(term_adl) { PARSE(term_adl); }
  SECTION(term_adm) { PARSE(term_adm); }
  SECTION(term_adn) { PARSE(term_adn); }
  SECTION(term_ado) { PARSE(term_ado); }
  SECTION(term_adp) { PARSE(term_adp); }
  SECTION(term_adq) { PARSE(term_adq); }
  SECTION(term_adr) { PARSE(term_adr); }
  SECTION(term_ads) { PARSE(term_ads); }

  SECTION(term_adt) { PARSE(term_adt); }
  SECTION(term_adu) { PARSE(term_adu); }
  SECTION(term_adv) { PARSE(term_adv); }
  SECTION(term_adw) { PARSE(term_adw); }
  SECTION(term_adx) { PARSE(term_adx); }
  SECTION(term_ady) { PARSE(term_ady); }
  SECTION(term_adz) { PARSE(term_adz); }

  SECTION(term_aea) { PARSE(term_aea); }
  SECTION(term_aeb) { PARSE(term_aeb); }
  SECTION(term_aec) { PARSE(term_aec); }
  SECTION(term_aed) { PARSE(term_aed); }
  SECTION(term_aee) { PARSE(term_aee); }
  SECTION(term_aef) { PARSE(term_aef); }
  SECTION(term_aeg) { PARSE(term_aeg); }
  SECTION(term_aeh) { PARSE(term_aeh); }
  SECTION(term_aei) { PARSE(term_aei); }
  SECTION(term_aej) { PARSE(term_aej); }
  SECTION(term_aek) { PARSE(term_aek); }
  SECTION(term_ael) { PARSE(term_ael); }
}
