#include "catch2/catch_test_macros.hpp"

#include "ast/action/Typecheck.h"

#include "aux/Environment.h"

#include "type/action/ToString.h"

#include "llvm/Support/InitLLVM.h"

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
  std::vector<std::string> source_lines = {
      "a := 108;\n",
      "b := true;\n",
      "c := false;\n",
      "d := nil;\n",
      "e := a;\n",
      "f := -36;\n",
      "g := 3 + 7;\n",
      "h := (f) + (12);\n",
      /*
      "i := [a, e, 2, f, g];\n",
      "j := (a, b, c, d, e);\n",
      "k := b = (true, false);\n",
      "l := b.0;\n",
      "m := b.0.1;\n",
      "n := b[0];\n",
      "o := b[0][1];\n",
      "p := b[0].1;\n",
      "q := b.1[2];\n",
      "r := b.1 + c[x];\n",

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
      */
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

  // the term [f := -36;] is not typechecking.
  // the error is unknown unop [-].
  // when lookup of [-] occurs, the two InternedStrings 
  // both hold "-", but with different addresses 
  // which causes Lookup to fail.
  { BIND_TERM_TYPE_IS(env.GetIntType()); }

  { BIND_TERM_TYPE_IS(env.GetIntType()); }

  { BIND_TERM_TYPE_IS(env.GetIntType()); }
}