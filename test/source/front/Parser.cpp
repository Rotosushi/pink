#include "catch2/catch_test_macros.hpp"

#include <iostream>

#include "front/Parser.h"

#include "aux/Environment.h"

#include "ast/All.h"

#include "llvm/Support/InitLLVM.h"

// okay, how can we test that the parser fails in the
// intended way? that is, returns an expected error
// given some input?

// unfortunately I belive these ~must~ be macros due to the
// use of Catch2 macros within their bodies. (free functions
// would not be able to use Catch2 macros properly, and neither
// would lambdas. (I think))
#define BIND_AFFIX_IS(affix_type)                                              \
  line_number        += 1;                                                     \
  auto &location     = locations[line_number - 1];                             \
  auto  parse_result = env.Parse();                                            \
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

#define FUNCTION_BODY_IS(body_type)                                            \
  line_number        += 1;                                                     \
  auto &location     = locations[line_number - 1];                             \
  auto  parse_result = env.Parse();                                            \
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

#define FUNCTION_ARGUMENT_TYPE_IS(argument_type)                               \
  line_number        += 1;                                                     \
  auto &location     = locations[line_number - 1];                             \
  auto  parse_result = env.Parse();                                            \
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
  REQUIRE(llvm::dyn_cast<pink::Variable>(cursor->get()));                      \
  auto &arguments = function->GetArguments();                                  \
  REQUIRE(arguments.size() == 1);                                              \
  REQUIRE(arguments[0].first == env.InternVariable("x"));                      \
  REQUIRE(arguments[0].second == (argument_type))

TEST_CASE("front/Parser", "[unit][front]") {
  /*
    We are getting memory leaks from the global structures
    that LLVM creates and we reference via the Environment class.

    so, it looks as if it is explicitly failing the test with the FAIL
    macro that is causing the leak. and not some code that I wrote.
    I don't know how I could resolve this unless there was some way to
    tell catch2 to run the destructors of the current scope on FAIL.
    so we'll call this Triaged for now.
  */

  /*
    The general idea is to ensure that all valid terms
    of the langauge we want to be semanitcally meaninfull,
    we want to be able to parse, so this *should* be a
    list of all valid forms in the language. (it probably
    isn't and will be updated with new and interesting test cases.)

    note that since we are just testing that everything Parses,
    we do not need to adhere to the typechecking rules.
  */
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

  { FUNCTION_ARGUMENT_TYPE_IS(env.GetNilType()); }

  { FUNCTION_ARGUMENT_TYPE_IS(env.GetIntType()); }

  { FUNCTION_ARGUMENT_TYPE_IS(env.GetBoolType()); }

  { FUNCTION_ARGUMENT_TYPE_IS(env.GetPointerType(env.GetIntType())); }

  { FUNCTION_ARGUMENT_TYPE_IS(env.GetSliceType(env.GetIntType())); }

  {
    FUNCTION_ARGUMENT_TYPE_IS(
        env.GetTupleType({env.GetIntType(), env.GetIntType()}));
  }

  { FUNCTION_ARGUMENT_TYPE_IS(env.GetArrayType(5, env.GetIntType())); }
}
