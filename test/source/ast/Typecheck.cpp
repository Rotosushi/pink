#include "catch2/catch_test_macros.hpp"

#include "ast/action/Typecheck.h"

#include "aux/Environment.h"

#include "type/action/ToString.h"

#include "llvm/Support/InitLLVM.h"

#define BIND_TERM_TYPE_IS(text, bind_term_type)                                \
  std::string    line{text};                                                   \
  pink::Location location{line_number, 0, line_number, line.length() - 1};     \
  line_number += 1;                                                            \
  std::stringstream stream{std::move(line)};                                   \
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
  CHECK(term_type == (bind_term_type));

TEST_CASE("ast/action/Typecheck", "[unit][ast][ast/action]") {
  unsigned line_number = 1;
  auto     env         = pink::Environment::CreateTestEnvironment();

  {BIND_TERM_TYPE_IS("a := nil;\n", env.GetNilType())}

  {BIND_TERM_TYPE_IS("b := 42;\n", env.GetIntType())}

  {BIND_TERM_TYPE_IS("c := true;\n", env.GetBoolType())}

  {BIND_TERM_TYPE_IS("d := false;\n", env.GetBoolType())}

  {BIND_TERM_TYPE_IS("e := a;\n", env.GetNilType())}

  {
    std::string    line{"f := -36;\n"};
    pink::Location location{line_number, 0, line_number, line.length() - 1};
    line_number += 1;
    std::stringstream stream{std::move(line)};
    env.SetIStream(&stream);
    auto parse_result = env.Parse();
    if (!parse_result.GetWhich()) {
      env.PrintErrorWithSourceText(std::cerr, parse_result.GetSecond());
      FAIL("Unable to parse expression.");
    }
    REQUIRE(parse_result);
    auto &expression = parse_result.GetFirst();
    CHECK(expression->GetLocation() == location);
    auto typecheck_result = Typecheck(expression, env);
    if (!typecheck_result.GetWhich()) {
      env.PrintErrorWithSourceText(std::cerr, typecheck_result.GetSecond());
      FAIL("Unable to type expression.");
    }
    auto term_type = typecheck_result.GetFirst();
    CHECK(term_type == (env.GetIntType()));
  }

  {BIND_TERM_TYPE_IS("g := 3 + 7;\n", env.GetIntType())}

  {BIND_TERM_TYPE_IS("a := (0) + (5);\n", env.GetIntType())}

  {
    const std::size_t five = 5;
    BIND_TERM_TYPE_IS("h := [0, 1, 2, 3, 4];\n",
                      env.GetArrayType(five, env.GetIntType()))
  }
}