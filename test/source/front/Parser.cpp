#include "catch2/catch_test_macros.hpp"

#include <iostream>

#include "front/Parser.h"

#include "aux/Environment.h"

#include "ast/All.h"

/*
  The general idea is to ensure that all valid terms
  of the langauge we want to be semanitcally meaninfull,
  we want to be able to parse, so this *should* be a
  list of all valid forms in the language. (it probably
  isn't and will be updated with new interesting test cases.)
*/
#define BIND_AFFIX_IS(text, affix_type)                                        \
  std::string       line{text};                                                \
  pink::Location    location{1, 0, 1, line.length() - 1};                      \
  std::stringstream stream{std::move(line)};                                   \
  env.SetIStream(&stream);                                                     \
  auto parse_result = env.Parse();                                             \
  if (!parse_result) {                                                         \
    env.PrintErrorWithSourceText(std::cerr, parse_result.GetSecond());         \
  }                                                                            \
  REQUIRE(parse_result);                                                       \
  auto *expression = parse_result.GetFirst().get();                            \
  CHECK(expression->GetLocation() == location);                                \
  auto *bind = llvm::dyn_cast<pink::Bind>(expression);                         \
  REQUIRE(bind != nullptr);                                                    \
  auto *affix = bind->GetAffix().get();                                        \
  REQUIRE(llvm::dyn_cast<affix_type>(affix) != nullptr);

#define FUNCTION_BODY_IS(text, body_type)                                      \
  std::string       line{text};                                                \
  pink::Location    location{1, 0, 1, line.length() - 1};                      \
  std::stringstream stream{std::move(line)};                                   \
  env.SetIStream(&stream);                                                     \
  auto parse_result = env.Parse();                                             \
  if (!parse_result) {                                                         \
    env.PrintErrorWithSourceText(std::cerr, parse_result.GetSecond());         \
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
  REQUIRE(llvm::dyn_cast<body_type>(cursor->get()));

#define FUNCTION_ARGUMENT_TYPE_IS(text, argument_type)                         \
  std::string       line{text};                                                \
  pink::Location    location{1, 0, 1, line.length() - 1};                      \
  std::stringstream stream{std::move(line)};                                   \
  env.SetIStream(&stream);                                                     \
  auto parse_result = env.Parse();                                             \
  if (!parse_result) {                                                         \
    env.PrintErrorWithSourceText(std::cerr, parse_result.GetSecond());         \
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
  REQUIRE(arguments[0].first == env.InternVariable("x"));                      \
  REQUIRE(arguments[0].second == (argument_type));

TEST_CASE("front/Parser", "[unit][front]") {
  auto env = pink::Environment::CreateNativeEnvironment();

  SECTION("a := 108;"){BIND_AFFIX_IS("a := 108;\n", pink::Integer)}

  SECTION("a := true;"){BIND_AFFIX_IS("a := true;\n", pink::Boolean)}

  SECTION("a := false;"){BIND_AFFIX_IS("a := false;\n", pink::Boolean)}

  SECTION("a := nil;"){BIND_AFFIX_IS("a := nil;\n", pink::Nil)}

  SECTION("a := x;"){BIND_AFFIX_IS("a := x;\n", pink::Variable)}

  SECTION("a := -36;"){BIND_AFFIX_IS("a := -36;\n", pink::Unop)}

  SECTION("a := 3 + 7;"){BIND_AFFIX_IS("a := 3 + 7;\n", pink::Binop)}

  SECTION("a := (0) + (5);"){BIND_AFFIX_IS("a := (0) + (5);\n", pink::Binop)}

  SECTION("a := [0, 1, 2, 3, 4];"){
      BIND_AFFIX_IS("a := [0, 1, 2, 3, 4];\n", pink::Array)}

  SECTION("a := (0, 1, 2, 3, 4);"){
      BIND_AFFIX_IS("a := (0, 1, 2, 3, 4);\n", pink::Tuple)}

  SECTION("a := b = (true, false);"){
      BIND_AFFIX_IS("a := b = (true, false);\n", pink::Assignment)}

  SECTION("a := b.0;"){BIND_AFFIX_IS("a := b.0;\n", pink::Dot)}

  SECTION("a := b.0.1;"){BIND_AFFIX_IS("a := b.0.1;\n", pink::Dot)}

  SECTION("a := b[0];"){BIND_AFFIX_IS("a := b[0];\n", pink::Subscript)}

  SECTION("a := b[0][1];"){BIND_AFFIX_IS("a := b[0][1];\n", pink::Subscript)}

  SECTION("a := b[0].1;"){BIND_AFFIX_IS("a := b[0].1;\n", pink::Dot)}

  SECTION("a := b.1[2];"){BIND_AFFIX_IS("a := b.1[2];\n", pink::Subscript)}

  SECTION("a := b.1 + c[x];"){BIND_AFFIX_IS("a := b.1 + c[x];\n", pink::Binop)}

  SECTION("fn f () { nil; }"){FUNCTION_BODY_IS("fn f () { nil; }\n", pink::Nil)}

  SECTION("fn f () { 42; }"){
      FUNCTION_BODY_IS("fn f () { 42; }\n", pink::Integer)}

  SECTION("fn f () { true; }"){
      FUNCTION_BODY_IS("fn f () { true; }\n", pink::Boolean)}

  SECTION("fn f () { false; }"){
      FUNCTION_BODY_IS("fn f () { false; }\n", pink::Boolean)}

  SECTION("fn f () { -23; }"){
      FUNCTION_BODY_IS("fn f () { -23; }\n", pink::Unop)}

  SECTION("fn f () { 7 * 8; }"){
      FUNCTION_BODY_IS("fn f () { 7 * 8; }\n", pink::Binop)}

  SECTION("fn f () { x; }"){
      FUNCTION_BODY_IS("fn f () { x; }\n", pink::Variable)}

  SECTION("fn f () { [0, 1, 2, 3, 4]; }"){
      FUNCTION_BODY_IS("fn f () { [0, 1, 2, 3, 4]; }\n", pink::Array)}

  SECTION("fn f () { (0, 1, 2, 3, 4); }"){
      FUNCTION_BODY_IS("fn f () { (0, 1, 2, 3, 4); }\n", pink::Tuple)}

  SECTION("fn f () { (5) - (3); }"){
      FUNCTION_BODY_IS("fn f () { (5) - (3); }\n", pink::Binop)}

  SECTION("fn f () { x = 56; }"){
      FUNCTION_BODY_IS("fn f () { x = 56; }\n", pink::Assignment)}

  SECTION("fn f () { x := 56; }"){
      FUNCTION_BODY_IS("fn f () { x := 56; }\n", pink::Bind)}

  SECTION("fn f () { if (true) { 10; } else { 11; } }"){
      FUNCTION_BODY_IS("fn f () { if (true) { 10; } else { 11; } }\n",
                       pink::Conditional)}

  SECTION("fn f () { while (true) do { x = x + 1; } }"){
      FUNCTION_BODY_IS("fn f () { while (true) do { x = x + 1; } }\n",
                       pink::While)}

  SECTION("fn f (x: Nil) { x; }"){
      FUNCTION_ARGUMENT_TYPE_IS("fn f (x: Nil) { x; }\n", env.GetNilType())}

  SECTION("fn f (x: Integer) { x; }"){
      FUNCTION_ARGUMENT_TYPE_IS("fn f (x: Integer) { x; }\n", env.GetIntType())}

  SECTION("fn f (x: Boolean) { x; }"){
      FUNCTION_ARGUMENT_TYPE_IS("fn f (x: Boolean) { x; }\n",
                                env.GetBoolType())}

  SECTION("fn f (x: Pointer Integer) { x; }"){
      FUNCTION_ARGUMENT_TYPE_IS("fn f (x: Pointer Integer) { x; }\n",
                                env.GetPointerType(env.GetIntType()))}

  SECTION("fn f (x: Slice Integer) { x; }"){
      FUNCTION_ARGUMENT_TYPE_IS("fn f (x: Slice Integer) { x; }\n",
                                env.GetSliceType(env.GetIntType()))}

  SECTION("fn f (x: (Integer, Integer)) { x; }"){FUNCTION_ARGUMENT_TYPE_IS(
      "fn f (x: (Integer, Integer)) { x; }\n",
      env.GetTupleType({env.GetIntType(), env.GetIntType()}))}

  SECTION("fn f (x: [Integer; 5]) { x; }") {
    FUNCTION_ARGUMENT_TYPE_IS("fn f (x: [Integer; 5]) { x; }\n",
                              env.GetArrayType(5, env.GetIntType()))
  }
}
