// Copyright (C) 2023 Cade Weinberg
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
#pragma once

/*
  This file is meant to hold all of the terms
  which we expect to be able to Parse and Typecheck.
  (for codegen we must wrap the terms to be tested
   within a main function, so we can test the emitted
   program.)

  an upside of this is that we have a single source of
  truth between Parse and Typecheck which reduces the
  amount of duplicate code that must be written.
  and it simplifies answering the question, have we
  tested all terms in the language

  an upside is also that each term can be associated
  with a unique identifer.

  the downside is that code which requires some setup before
  the test can be validly done will become more opaque.
  with programmer having to use this file for reference
  to write and debug tests.

  This list is not yet comprehensive

  \todo write tests for expected failure states.
*/

constexpr inline auto term_aaa = "aaa := 30;";
constexpr inline auto term_aab = "aab := true;";
constexpr inline auto term_aac = "aac := (false);";
constexpr inline auto term_aad = "aad := nil;";
constexpr inline auto term_aae = "aae := a;";
constexpr inline auto term_aaf = "aaf := -1;";
constexpr inline auto term_aag = "aag := !true;";
constexpr inline auto term_aah = "aah := &a;";
constexpr inline auto term_aai = "aai := *a;";

constexpr inline auto term_aaj = "aaj := 30 + 5;";
constexpr inline auto term_aak = "aak := 30 - 5;";
constexpr inline auto term_aal = "aal := 30 * 5;";
constexpr inline auto term_aam = "aam := 30 / 5;";
constexpr inline auto term_aan = "aan := 30 % 5;";
constexpr inline auto term_aao = "aao := true & true;";
constexpr inline auto term_aap = "aap := true | false;";
constexpr inline auto term_aaq = "aaq := 30 == 5;";
constexpr inline auto term_aar = "aar := true == false;";
constexpr inline auto term_aas = "aas := 30 != 5;";
constexpr inline auto term_aat = "aat := true != false;";
constexpr inline auto term_aau = "aau := 30 < 5;";
constexpr inline auto term_aav = "aav := 30 <= 5;";
constexpr inline auto term_aaw = "aaw := 30 > 5;";
constexpr inline auto term_aax = "aax := 30 >= 5;";

constexpr inline auto term_aay = "aay := -30 + -5;";
constexpr inline auto term_aaz = "aaz := -(30 + 5);";
constexpr inline auto term_aba = "aba := !true | !false;";
constexpr inline auto term_abb = "abb := !(true | false);";
constexpr inline auto term_abc = "abc := -30 + 5 == -5 + 30;";
constexpr inline auto term_abd = "abd := 30 - -5 * 6 == 90 - 30;";

constexpr inline auto term_abe = "abe := a = 42;";
constexpr inline auto term_abf = "abf := tuple.0;";
constexpr inline auto term_abg = "abg := array[x];";
constexpr inline auto term_abh = "abh := tuple.1 = 30;";
constexpr inline auto term_abi = "abi := array[x] = 30;";
constexpr inline auto term_abj = "abj := tuple.0 + 5;";
constexpr inline auto term_abk = "abk := array[x] + 5;";
constexpr inline auto term_abl = "abl := tuple.0.1;";
constexpr inline auto term_abm = "abm := tuple.0[x];";
constexpr inline auto term_abn = "abn := array[x][y];";
constexpr inline auto term_abo = "abo := array[x].0;";
constexpr inline auto term_abp = "abp := tuple.0 + array[x];";
constexpr inline auto term_abq = "abq := -tuple.0;";
constexpr inline auto term_abr = "abr := !array[x];";

constexpr inline auto term_abt = "abt := f;";
constexpr inline auto term_abu = "abu := f();";
constexpr inline auto term_abv = "abv := f(0);";
constexpr inline auto term_abw = "abw := f(0, 1);";
constexpr inline auto term_abx = "abx := f(1, 2) + g(3, 4);";
constexpr inline auto term_aby = "aby := tuple.0();";
constexpr inline auto term_abz = "abz := array[x]();";

constexpr inline auto term_aca = "aac := [0, 1, 2, 3, 4];";
constexpr inline auto term_acb = "acb := [[0, 1], [2, 3], [4, 5]];";
constexpr inline auto term_acc = "acc := (0, 1);";
constexpr inline auto term_acd = "acd := ((0, 1), (2, 3));";
constexpr inline auto term_ace = "ace := [(0, 1), (2, 3), (4, 5)];";
constexpr inline auto term_acf = "acf := ([0, 1, 2], [3, 4, 5]);";

constexpr inline auto term_acg = "fn acg() { 30; }";
constexpr inline auto term_ach = "fn ach() { true; }";
constexpr inline auto term_aci = "fn aci() { false; }";
constexpr inline auto term_acj = "fn acj() { nil; }";
constexpr inline auto term_ack = "fn ack() { -5; }";
constexpr inline auto term_acl = "fn acl() { !true; }";
constexpr inline auto term_acm = "fn acm() { &a; }";
constexpr inline auto term_acn = "fn acn() { *a; }";

constexpr inline auto term_aco = "fn aco() { 30 + 5; }";
constexpr inline auto term_acp = "fn acp() { 30 - 5; }";
constexpr inline auto term_acq = "fn acq() { 30 * 5; }";
constexpr inline auto term_acr = "fn acr() { 30 / 5; }";
constexpr inline auto term_acs = "fn acs() { 30 % 5; }";
constexpr inline auto term_act = "fn act() { true & false; }";
constexpr inline auto term_acu = "fn acu() { false | true; }";
constexpr inline auto term_acv = "fn acv() { 30 == 5; }";
constexpr inline auto term_acw = "fn acw() { 30 != 5; }";
constexpr inline auto term_acx = "fn acx() { true == false; }";
constexpr inline auto term_acy = "fn acy() { true != false; }";
constexpr inline auto term_acz = "fn acz() { 30 < 5; }";
constexpr inline auto term_ada = "fn ada() { 30 <= 5; }";
constexpr inline auto term_adb = "fn adb() { 30 > 5; }";
constexpr inline auto term_adc = "fn adc() { 30 >= 5; }";

constexpr inline auto term_add = "fn add() { a; }";
constexpr inline auto term_ade = "fn ade() { a = 30; }";
constexpr inline auto term_adf = "fn adf() { [0, 1, 2, 3, 4]; }";
constexpr inline auto term_adg = "fn adg() { [[0, 1], [2, 3], [4, 5]]; }";
constexpr inline auto term_adh = "fn adh() { (0, 1); }";
constexpr inline auto term_adi = "fn adi() { ((0, 1), (2, 3)); }";
constexpr inline auto term_adj = "fn adj() { [(0, 1), (2, 3), (4, 5)]; }";
constexpr inline auto term_adk = "fn adk() { ([0, 1, 2], [3, 4, 5]); }";

constexpr inline auto term_adl = "fn adl() { array[x]; }";
constexpr inline auto term_adm = "fn adm() { array[x][y]; }";
constexpr inline auto term_adn = "fn adn() { tuple.0; }";
constexpr inline auto term_ado = "fn ado() { tuple.0.1; }";
constexpr inline auto term_adp = "fn adp() { array[x].0; }";
constexpr inline auto term_adq = "fn adq() { tuple.0[x]; }";
constexpr inline auto term_adr = "fn adr() { -array[x]; }";
constexpr inline auto term_ads = "fn ads() { !tuple.0; }";

constexpr inline auto term_adt = "fn adt() { f; }";
constexpr inline auto term_adu = "fn adu() { f(); }";
constexpr inline auto term_adv = "fn adv() { f(0); }";
constexpr inline auto term_adw = "fn adw() { f(0, 1); }";
constexpr inline auto term_adx = "fn adx() { f(1, 2) + g(3, 4); }";
constexpr inline auto term_ady = "fn ady() { array[x](); }";
constexpr inline auto term_adz = "fn adz() { tuple.0(); }";

constexpr inline auto term_aea = "fn aea(a: Integer) { a; }";
constexpr inline auto term_aeb = "fn aeb(a: Boolean) { a; }";
constexpr inline auto term_aec = "fn aec(a: Nil) { a; }";
constexpr inline auto term_aed = "fn aed(a: *Integer) { a; }";
constexpr inline auto term_aee = "fn aee(a: *[]Integer) { a; }";
constexpr inline auto term_aef = "fn aef(a: (Integer, Integer)) { a; }";
constexpr inline auto term_aeg = "fn aeg(a: [Integer; 10]) { a; }";
constexpr inline auto term_aeh = "fn aeh(a: **Integer) { a; }";
constexpr inline auto term_aei = "fn aei(a: **[]Integer) { a; }";
constexpr inline auto term_aej = "fn aej(a: *[]*[]Integer) { a; }";
constexpr inline auto term_aek = "fn aek(a: *[]*Integer) { a; }";
constexpr inline auto term_ael = "fn ael(a: fn() -> Integer) { a; }";

constexpr inline auto term_aem = "fn aem(a: Integer, b: Integer) { a + b; }";
constexpr inline auto term_aen = "fn aen(a: Boolean, b: Boolean) { a & b; }";
constexpr inline auto term_aeo = "fn aeo(a: *Integer, b: Integer) { *a + b; }";
constexpr inline auto term_aep =
    "fn aep(a: *[]Integer, x: Integer, b: Integer) { a[x] + b; }";
constexpr inline auto term_aeq = "fn aeq(a: (Integer, Integer)) { a.0 + a.1; }";
constexpr inline auto term_aer =
    "fn aer(a: [Integer; 10], x: Integer, b: Integer) { a[x] + b; }";
constexpr inline auto term_aes =
    "fn aes(a: **Integer, b: Integer) { **a + b; }";
constexpr inline auto term_aet =
    "fn aet(a: **[]Integer, x: Integer, b: Integer) { (*a)[x] + b; }";
constexpr inline auto term_aeu =
    "fn aeu(a: *[]*[]Integer, x: Integer, y: Integer) { a[x][y]; }";
constexpr inline auto term_aev =
    "fn aev(a: *[]*Integer, x: Integer, b: Integer) { *a[x] + b; }";
constexpr inline auto term_aew = "fn aew(a: fn() -> Integer) { a(); }";
constexpr inline auto term_aex = "fn aex(a: fn(Integer, Integer) -> Integer, "
                                 "b: Integer, c: Integer) { a(b, c); }";
