#include "catch2/catch_test_macros.hpp"

#include "ast/action/Typecheck.h"

#include "aux/Environment.h"

#include "type/action/ToString.h"

#define BIND_TERM_TYPE_IS(text, bind_term_type)                                \
  std::string       line{text};                                                \
  pink::Location    location{1, 0, 1, line.length() - 1};                      \
  std::stringstream stream{std::move(line)};                                   \
  env.SetIStream(&stream);                                                     \
  auto parse_result = env.Parse();                                             \
  if (!parse_result.GetWhich()) {                                              \
    env.PrintErrorWithSourceText(std::cerr, parse_result.GetSecond());         \
  }                                                                            \
  REQUIRE(parse_result);                                                       \
  auto &expression = parse_result.GetFirst();                                  \
  CHECK(expression->GetLocation() == location);                                \
  auto typecheck_result = Typecheck(expression, env);                          \
  if (!typecheck_result.GetWhich()) {                                          \
    env.PrintErrorWithSourceText(std::cerr, typecheck_result.GetSecond());     \
  }                                                                            \
  auto term_type = typecheck_result.GetFirst();                                \
  CHECK(term_type == (bind_term_type));

TEST_CASE("ast/action/Typecheck", "[unit][ast][ast/action]") {
  auto env = pink::Environment::CreateNativeEnvironment();

  SECTION("a := nil;"){
      BIND_TERM_TYPE_IS("a := nil;\n", env.GetNilType())} SECTION("a := 42;"){
      BIND_TERM_TYPE_IS("a := 42;\n", env.GetIntType())} SECTION("a := true;"){
      BIND_TERM_TYPE_IS("a := true;\n",
                        env.GetBoolType())} SECTION("a := false;"){
      BIND_TERM_TYPE_IS("a := false;\n",
                        env.GetBoolType())} SECTION("a := x;") {
    env.DeclareVariable("x", env.GetNilType());
    BIND_TERM_TYPE_IS("a := x;\n", env.GetNilType())
  }
  SECTION("a := -36;") {
    std::string       line{"a := -36;\n"};
    pink::Location    location{1, 0, 1, line.length() - 1};
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
  SECTION("a := 3 + 7;"){
      BIND_TERM_TYPE_IS("a := 3 + 7;\n",
                        env.GetIntType())} SECTION("a := (0) + (5);"){
      BIND_TERM_TYPE_IS("a := (0) + (5);\n",
                        env.GetIntType())} std::size_t five = 5;
  SECTION("a := [0, 1, 2, 3, 4];") {

    BIND_TERM_TYPE_IS("a := [0, 1, 2, 3, 4];\n",
                      env.GetArrayType(five, env.GetIntType()))
  }
}