#include "catch2/catch_test_macros.hpp"

#include <iostream>

#include "front/Parser.h"

#include "aux/Environment.h"

#include "ast/All.h"

#include "llvm/Support/InitLLVM.h"

/*
  The general idea is to ensure that all valid terms
  of the langauge we want to be semanitcally meaninfull,
  we want to be able to parse, so this *should* be a
  list of all valid forms in the language. (it probably
  isn't and will be updated with new interesting test cases.)
*/
#define BIND_AFFIX_IS(text, affix_type)                                        \
  std::string_view line{text};                                                 \
  pink::Location   location{line_number, 0, line_number, line.length() - 1};   \
  line_number += 1;                                                            \
  env.SetBuffer(line);                                                         \
  auto parse_result = env.Parse();                                             \
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
  REQUIRE(llvm::dyn_cast<affix_type>(affix) != nullptr);

#define FUNCTION_BODY_IS(text, body_type)                                      \
  std::string_view line{text};                                                 \
  pink::Location   location{line_number, 0, line_number, line.length() - 1};   \
  line_number += 1;                                                            \
  env.SetBuffer(line);                                                         \
  auto parse_result = env.Parse();                                             \
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
  REQUIRE(llvm::dyn_cast<body_type>(cursor->get()));

#define FUNCTION_ARGUMENT_TYPE_IS(text, argument_type)                         \
  std::string_view line{text};                                                 \
  pink::Location   location{line_number, 0, line_number, line.length() - 1};   \
  line_number += 1;                                                            \
  env.SetBuffer(line);                                                         \
  auto parse_result = env.Parse();                                             \
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
  REQUIRE(arguments[0].second == (argument_type));

TEST_CASE("front/Parser", "[unit][front]") {
  /*
    We are getting memory leaks from the global structures
    that LLVM creates and we reference via the Environment class.
    When the test exits normally and when the test exits with FAIL()
    We need to rethink how we test Parse, Typecheck, and Codegen
    so as to remove these leaks.
    1) it does not work to call InitLLVM once at the start of main,
      (though it seems like this would be the solution as these are
      ManagedStatic objects that are leaking.)
    2) it does not work to call InitLLVM in each test

    do we still get leaks if we do not initialize the llvm::*
    members? yes, the llvm::StringSet of the StringInterner leaks,
    as does the Ast's which are constructed.
    my biggest question is why do std containers not leak when 
    returning via FAIL and the llvm containers do seem to leak?
    if it is an abnormal return where destructors are not called 
    then prsumably both would leak as destructors are not called.
    and if it is a normal return where destructors are called 
    then why does llvm containers leak at all? 
    next question, does the main program leak with the same containers?
    because presumably the llvm::InitLLVM class is working as intended 
    when we use it in the main program.
    (namely init at begin of main, destructor at end of main cleans up 
    managed static objects.)
  */
  unsigned line_number = 1;
  auto     env         = pink::Environment::CreateTestEnvironment();

  {BIND_AFFIX_IS("a := 108;\n", pink::Integer)}

  {BIND_AFFIX_IS("a := true;\n", pink::Boolean)}

  {BIND_AFFIX_IS("a := false;\n", pink::Boolean)}

  {BIND_AFFIX_IS("a := nil;\n", pink::Nil)}

  {BIND_AFFIX_IS("a := x;\n", pink::Variable)}

  {BIND_AFFIX_IS("a := -36;\n", pink::Unop)}

  {BIND_AFFIX_IS("a := 3 + 7;\n", pink::Binop)}

  {BIND_AFFIX_IS("a := (0) + (5);\n", pink::Binop)}

  {BIND_AFFIX_IS("a := [0, 1, 2, 3, 4];\n", pink::Array)}

  {BIND_AFFIX_IS("a := (0, 1, 2, 3, 4);\n", pink::Tuple)}

  {BIND_AFFIX_IS("a := b = (true, false);\n", pink::Assignment)}

  {BIND_AFFIX_IS("a := b.0;\n", pink::Dot)}

  {BIND_AFFIX_IS("a := b.0.1;\n", pink::Dot)}

  {BIND_AFFIX_IS("a := b[0];\n", pink::Subscript)}

  {BIND_AFFIX_IS("a := b[0][1];\n", pink::Subscript)}

  {BIND_AFFIX_IS("a := b[0].1;\n", pink::Dot)}

  {BIND_AFFIX_IS("a := b.1[2];\n", pink::Subscript)}

  {BIND_AFFIX_IS("a := b.1 + c[x];\n", pink::Binop)}

  {FUNCTION_BODY_IS("fn f () { nil; }\n", pink::Nil)}

  {FUNCTION_BODY_IS("fn f () { 42; }\n", pink::Integer)}

  {FUNCTION_BODY_IS("fn f () { true; }\n", pink::Boolean)}

  {FUNCTION_BODY_IS("fn f () { false; }\n", pink::Boolean)}

  {FUNCTION_BODY_IS("fn f () { -23; }\n", pink::Unop)}

  {FUNCTION_BODY_IS("fn f () { 7 * 8; }\n", pink::Binop)}

  {FUNCTION_BODY_IS("fn f () { x; }\n", pink::Variable)}

  {FUNCTION_BODY_IS("fn f () { [0, 1, 2, 3, 4]; }\n", pink::Array)}

  {FUNCTION_BODY_IS("fn f () { (0, 1, 2, 3, 4); }\n", pink::Tuple)}

  {FUNCTION_BODY_IS("fn f () { (5) - (3); }\n", pink::Binop)}

  {FUNCTION_BODY_IS("fn f () { x = 56; }\n", pink::Assignment)}

  {FUNCTION_BODY_IS("fn f () { x := 56; }\n", pink::Bind)}

  {FUNCTION_BODY_IS("fn f () { if (true) { 10; } else { 11; } }\n",
                    pink::IfThenElse)}

  {FUNCTION_BODY_IS("fn f () { while (true) do { x = x + 1; } }\n",
                    pink::While)}

  {FUNCTION_ARGUMENT_TYPE_IS("fn f (x: Nil) { x; }\n", env.GetNilType())}

  {FUNCTION_ARGUMENT_TYPE_IS("fn f (x: Integer) { x; }\n", env.GetIntType())}

  {FUNCTION_ARGUMENT_TYPE_IS("fn f (x: Boolean) { x; }\n", env.GetBoolType())}

  {FUNCTION_ARGUMENT_TYPE_IS("fn f (x: Pointer Integer) { x; }\n",
                             env.GetPointerType(env.GetIntType()))}

  {FUNCTION_ARGUMENT_TYPE_IS("fn f (x: Slice Integer) { x; }\n",
                             env.GetSliceType(env.GetIntType()))}

  {FUNCTION_ARGUMENT_TYPE_IS(
      "fn f (x: (Integer, Integer)) { x; }\n",
      env.GetTupleType({env.GetIntType(), env.GetIntType()}))}

  {
    FUNCTION_ARGUMENT_TYPE_IS("fn f (x: [Integer; 5]) { x; }\n",
                              env.GetArrayType(5, env.GetIntType()))
  }
}
