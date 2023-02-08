#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers.hpp"

#include "front/Parser.h"

#include "aux/Environment.h"

#include "ast/All.h"

TEST_CASE("front/Parser", "[unit][front]") {
  auto env = pink::Environment::CreateNativeGlobalEnv();

  SECTION("a := 108;") {
    std::string       line{"a := 108;\n"};
    pink::Location    location{1, 0, 1, line.length() - 1};
    std::stringstream stream{std::move(line)};
    env.parser.SetIStream(&stream);
    auto parse_result = env.parser.Parse(env);
    REQUIRE(parse_result);
    auto *expression = parse_result.GetFirst().get();
    CHECK(expression->GetLocation() == location);
    auto *bind = llvm::dyn_cast<pink::Bind>(expression);
    REQUIRE(bind != nullptr);
    auto *affix = bind->GetAffix().get();
    REQUIRE(llvm::dyn_cast<pink::Integer>(affix) != nullptr);
  }

  SECTION("a := true;") {
    std::string       line{"a := true;\n"};
    pink::Location    location{1, 0, 1, line.length() - 1};
    std::stringstream stream{std::move(line)};
    env.parser.SetIStream(&stream);
    auto parse_result = env.parser.Parse(env);
    REQUIRE(parse_result);
    auto *expression = parse_result.GetFirst().get();
    CHECK(expression->GetLocation() == location);
    auto *bind = llvm::dyn_cast<pink::Bind>(expression);
    REQUIRE(bind != nullptr);
    auto *affix = bind->GetAffix().get();
    REQUIRE(llvm::dyn_cast<pink::Boolean>(affix) != nullptr);
  }

  SECTION("a := false;") {
    std::string       line{"a := false;\n"};
    pink::Location    location{1, 0, 1, line.length() - 1};
    std::stringstream stream{std::move(line)};
    env.parser.SetIStream(&stream);
    auto parse_result = env.parser.Parse(env);
    REQUIRE(parse_result);
    auto *expression = parse_result.GetFirst().get();
    CHECK(expression->GetLocation() == location);
    auto *bind = llvm::dyn_cast<pink::Bind>(expression);
    REQUIRE(bind != nullptr);
    auto *affix = bind->GetAffix().get();
    REQUIRE(llvm::dyn_cast<pink::Boolean>(affix) != nullptr);
  }

  SECTION("a := nil;") {
    std::string       line{"a := nil;\n"};
    pink::Location    location{1, 0, 1, line.length() - 1};
    std::stringstream stream{std::move(line)};
    env.parser.SetIStream(&stream);
    auto parse_result = env.parser.Parse(env);
    REQUIRE(parse_result);
    auto *expression = parse_result.GetFirst().get();
    CHECK(expression->GetLocation() == location);
    auto *bind = llvm::dyn_cast<pink::Bind>(expression);
    REQUIRE(bind != nullptr);
    auto *affix = bind->GetAffix().get();
    REQUIRE(llvm::dyn_cast<pink::Nil>(affix) != nullptr);
  }

  SECTION("a := x;") {
    std::string       line{"a := x;\n"};
    pink::Location    location{1, 0, 1, line.length() - 1};
    std::stringstream stream{std::move(line)};
    env.parser.SetIStream(&stream);
    auto parse_result = env.parser.Parse(env);
    REQUIRE(parse_result);
    auto *expression = parse_result.GetFirst().get();
    CHECK(expression->GetLocation() == location);
    auto *bind = llvm::dyn_cast<pink::Bind>(expression);
    REQUIRE(bind != nullptr);
    auto *affix = bind->GetAffix().get();
    REQUIRE(llvm::dyn_cast<pink::Variable>(affix) != nullptr);
  }

  SECTION("a := -36;") {
    std::string       line{"a := -36;\n"};
    pink::Location    location{1, 0, 1, line.length() - 1};
    std::stringstream stream{std::move(line)};
    env.parser.SetIStream(&stream);
    auto parse_result = env.parser.Parse(env);
    REQUIRE(parse_result);
    auto *expression = parse_result.GetFirst().get();
    CHECK(expression->GetLocation() == location);
    auto *bind = llvm::dyn_cast<pink::Bind>(expression);
    REQUIRE(bind != nullptr);
    auto *affix = bind->GetAffix().get();
    REQUIRE(llvm::dyn_cast<pink::Unop>(affix) != nullptr);
  }

  SECTION("a := 3 + 7;") {
    std::string       line{"a := 3 + 7;\n"};
    pink::Location    location{1, 0, 1, line.length() - 1};
    std::stringstream stream{std::move(line)};
    env.parser.SetIStream(&stream);
    auto parse_result = env.parser.Parse(env);
    REQUIRE(parse_result);
    auto *expression = parse_result.GetFirst().get();
    CHECK(expression->GetLocation() == location);
    auto *bind = llvm::dyn_cast<pink::Bind>(expression);
    REQUIRE(bind != nullptr);
    auto *affix = bind->GetAffix().get();
    REQUIRE(llvm::dyn_cast<pink::Binop>(affix) != nullptr);
  }

  SECTION("a := [0, 1, 2, 3, 4];") {
    std::string       line{"a := [0, 1, 2, 3, 4];\n"};
    pink::Location    location{1, 0, 1, line.length() - 1};
    std::stringstream stream{std::move(line)};
    env.parser.SetIStream(&stream);
    auto parse_result = env.parser.Parse(env);
    REQUIRE(parse_result);
    auto *expression = parse_result.GetFirst().get();
    CHECK(expression->GetLocation() == location);
    auto *bind = llvm::dyn_cast<pink::Bind>(expression);
    REQUIRE(bind != nullptr);
    auto *affix = bind->GetAffix().get();
    REQUIRE(llvm::dyn_cast<pink::Array>(affix) != nullptr);
  }

  SECTION("a := (0, 1, 2, 3, 4);") {
    std::string       line{"a := (0, 1, 2, 3, 4);\n"};
    pink::Location    location{1, 0, 1, line.length() - 1};
    std::stringstream stream{std::move(line)};
    env.parser.SetIStream(&stream);
    auto parse_result = env.parser.Parse(env);
    REQUIRE(parse_result);
    auto *expression = parse_result.GetFirst().get();
    CHECK(expression->GetLocation() == location);
    auto *bind = llvm::dyn_cast<pink::Bind>(expression);
    REQUIRE(bind != nullptr);
    auto *affix = bind->GetAffix().get();
    REQUIRE(llvm::dyn_cast<pink::Tuple>(affix) != nullptr);
  }

  SECTION("a := (0) + (5);") {
    std::string       line{"a := (0) + (5);\n"};
    pink::Location    location{1, 0, 1, line.length() - 1};
    std::stringstream stream{std::move(line)};
    env.parser.SetIStream(&stream);
    auto parse_result = env.parser.Parse(env);
    REQUIRE(parse_result);
    auto *expression = parse_result.GetFirst().get();
    CHECK(expression->GetLocation() == location);
    auto *bind = llvm::dyn_cast<pink::Bind>(expression);
    REQUIRE(bind != nullptr);
    auto *affix = bind->GetAffix().get();
    REQUIRE(llvm::dyn_cast<pink::Binop>(affix) != nullptr);
  }

  SECTION("a := b = (true, false);") {
    std::string       line{"a := b = (true, false);\n"};
    pink::Location    location{1, 0, 1, line.length() - 1};
    std::stringstream stream{std::move(line)};
    env.parser.SetIStream(&stream);
    auto parse_result = env.parser.Parse(env);
    REQUIRE(parse_result);
    auto *expression = parse_result.GetFirst().get();
    CHECK(expression->GetLocation() == location);
    auto *bind = llvm::dyn_cast<pink::Bind>(expression);
    REQUIRE(bind != nullptr);
    auto *affix = bind->GetAffix().get();
    REQUIRE(llvm::dyn_cast<pink::Assignment>(affix) != nullptr);
  }

  SECTION("fn f () { nil; }") {
    std::string       line{"fn f () { nil; }\n"};
    pink::Location    location{1, 0, 1, line.length() - 1};
    std::stringstream stream{std::move(line)};
    env.parser.SetIStream(&stream);
    auto parse_result = env.parser.Parse(env);
    REQUIRE(parse_result);
    auto *expression = parse_result.GetFirst().get();
    CHECK(expression->GetLocation() == location);
    auto *function = llvm::dyn_cast<pink::Function>(expression);
    REQUIRE(function != nullptr);
    auto *block = llvm::dyn_cast<pink::Block>(function->GetBody().get());
    REQUIRE(block != nullptr);
    auto cursor = block->begin();
    REQUIRE(cursor != block->end());
    REQUIRE(llvm::dyn_cast<pink::Nil>(cursor->get()));
  }

  SECTION("fn f () { 42; }") {
    std::string       line{"fn f () { 42; }\n"};
    pink::Location    location{1, 0, 1, line.length() - 1};
    std::stringstream stream{std::move(line)};
    env.parser.SetIStream(&stream);
    auto parse_result = env.parser.Parse(env);
    REQUIRE(parse_result);
    auto *expression = parse_result.GetFirst().get();
    CHECK(expression->GetLocation() == location);
    auto *function = llvm::dyn_cast<pink::Function>(expression);
    REQUIRE(function != nullptr);
    auto *block = llvm::dyn_cast<pink::Block>(function->GetBody().get());
    REQUIRE(block != nullptr);
    auto cursor = block->begin();
    REQUIRE(cursor != block->end());
    REQUIRE(llvm::dyn_cast<pink::Integer>(cursor->get()));
  }

  SECTION("fn f () { true; }") {
    std::string       line{"fn f () { true; }\n"};
    pink::Location    location{1, 0, 1, line.length() - 1};
    std::stringstream stream{std::move(line)};
    env.parser.SetIStream(&stream);
    auto parse_result = env.parser.Parse(env);
    REQUIRE(parse_result);
    auto *expression = parse_result.GetFirst().get();
    CHECK(expression->GetLocation() == location);
    auto *function = llvm::dyn_cast<pink::Function>(expression);
    REQUIRE(function != nullptr);
    auto *block = llvm::dyn_cast<pink::Block>(function->GetBody().get());
    REQUIRE(block != nullptr);
    auto cursor = block->begin();
    REQUIRE(cursor != block->end());
    REQUIRE(llvm::dyn_cast<pink::Boolean>(cursor->get()));
  }

  SECTION("fn f () { false; }") {
    std::string       line{"fn f () { false; }\n"};
    pink::Location    location{1, 0, 1, line.length() - 1};
    std::stringstream stream{std::move(line)};
    env.parser.SetIStream(&stream);
    auto parse_result = env.parser.Parse(env);
    REQUIRE(parse_result);
    auto *expression = parse_result.GetFirst().get();
    CHECK(expression->GetLocation() == location);
    auto *function = llvm::dyn_cast<pink::Function>(expression);
    REQUIRE(function != nullptr);
    auto *block = llvm::dyn_cast<pink::Block>(function->GetBody().get());
    REQUIRE(block != nullptr);
    auto cursor = block->begin();
    REQUIRE(cursor != block->end());
    REQUIRE(llvm::dyn_cast<pink::Boolean>(cursor->get()));
  }

  SECTION("fn f () { -23; }") {
    std::string       line{"fn f () { -23; }\n"};
    pink::Location    location{1, 0, 1, line.length() - 1};
    std::stringstream stream{std::move(line)};
    env.parser.SetIStream(&stream);
    auto parse_result = env.parser.Parse(env);
    REQUIRE(parse_result);
    auto *expression = parse_result.GetFirst().get();
    CHECK(expression->GetLocation() == location);
    auto *function = llvm::dyn_cast<pink::Function>(expression);
    REQUIRE(function != nullptr);
    auto *block = llvm::dyn_cast<pink::Block>(function->GetBody().get());
    REQUIRE(block != nullptr);
    auto cursor = block->begin();
    REQUIRE(cursor != block->end());
    REQUIRE(llvm::dyn_cast<pink::Unop>(cursor->get()));
  }

  SECTION("fn f () { 7 * 8; }") {
    std::string       line{"fn f () { 7 * 8; }\n"};
    pink::Location    location{1, 0, 1, line.length() - 1};
    std::stringstream stream{std::move(line)};
    env.parser.SetIStream(&stream);
    auto parse_result = env.parser.Parse(env);
    REQUIRE(parse_result);
    auto *expression = parse_result.GetFirst().get();
    CHECK(expression->GetLocation() == location);
    auto *function = llvm::dyn_cast<pink::Function>(expression);
    REQUIRE(function != nullptr);
    auto *block = llvm::dyn_cast<pink::Block>(function->GetBody().get());
    REQUIRE(block != nullptr);
    auto cursor = block->begin();
    REQUIRE(cursor != block->end());
    REQUIRE(llvm::dyn_cast<pink::Binop>(cursor->get()));
  }

  SECTION("fn f () { x; }") {
    std::string       line{"fn f () { x; }\n"};
    pink::Location    location{1, 0, 1, line.length() - 1};
    std::stringstream stream{std::move(line)};
    env.parser.SetIStream(&stream);
    auto parse_result = env.parser.Parse(env);
    REQUIRE(parse_result);
    auto *expression = parse_result.GetFirst().get();
    CHECK(expression->GetLocation() == location);
    auto *function = llvm::dyn_cast<pink::Function>(expression);
    REQUIRE(function != nullptr);
    auto *block = llvm::dyn_cast<pink::Block>(function->GetBody().get());
    REQUIRE(block != nullptr);
    auto cursor = block->begin();
    REQUIRE(cursor != block->end());
    REQUIRE(llvm::dyn_cast<pink::Variable>(cursor->get()));
  }

  SECTION("fn f () { [0, 1, 2, 3, 4]; }") {
    std::string       line{"fn f () { [0, 1, 2, 3, 4]; }\n"};
    pink::Location    location{1, 0, 1, line.length() - 1};
    std::stringstream stream{std::move(line)};
    env.parser.SetIStream(&stream);
    auto parse_result = env.parser.Parse(env);
    REQUIRE(parse_result);
    auto *expression = parse_result.GetFirst().get();
    CHECK(expression->GetLocation() == location);
    auto *function = llvm::dyn_cast<pink::Function>(expression);
    REQUIRE(function != nullptr);
    auto *block = llvm::dyn_cast<pink::Block>(function->GetBody().get());
    REQUIRE(block != nullptr);
    auto cursor = block->begin();
    REQUIRE(cursor != block->end());
    REQUIRE(llvm::dyn_cast<pink::Array>(cursor->get()));
  }

  SECTION("fn f () { (0, 1, 2, 3, 4); }") {
    std::string       line{"fn f () { (0, 1, 2, 3, 4); }\n"};
    pink::Location    location{1, 0, 1, line.length() - 1};
    std::stringstream stream{std::move(line)};
    env.parser.SetIStream(&stream);
    auto parse_result = env.parser.Parse(env);
    REQUIRE(parse_result);
    auto *expression = parse_result.GetFirst().get();
    CHECK(expression->GetLocation() == location);
    auto *function = llvm::dyn_cast<pink::Function>(expression);
    REQUIRE(function != nullptr);
    auto *block = llvm::dyn_cast<pink::Block>(function->GetBody().get());
    REQUIRE(block != nullptr);
    auto cursor = block->begin();
    REQUIRE(cursor != block->end());
    REQUIRE(llvm::dyn_cast<pink::Tuple>(cursor->get()));
  }

  SECTION("fn f () { (5) - (3); }") {
    std::string       line{"fn f () { (5) - (3); }\n"};
    pink::Location    location{1, 0, 1, line.length() - 1};
    std::stringstream stream{std::move(line)};
    env.parser.SetIStream(&stream);
    auto parse_result = env.parser.Parse(env);
    REQUIRE(parse_result);
    auto *expression = parse_result.GetFirst().get();
    CHECK(expression->GetLocation() == location);
    auto *function = llvm::dyn_cast<pink::Function>(expression);
    REQUIRE(function != nullptr);
    auto *block = llvm::dyn_cast<pink::Block>(function->GetBody().get());
    REQUIRE(block != nullptr);
    auto cursor = block->begin();
    REQUIRE(cursor != block->end());
    REQUIRE(llvm::dyn_cast<pink::Binop>(cursor->get()));
  }

  SECTION("fn f () { x = 56; }") {
    std::string       line{"fn f () { x = 56; }\n"};
    pink::Location    location{1, 0, 1, line.length() - 1};
    std::stringstream stream{std::move(line)};
    env.parser.SetIStream(&stream);
    auto parse_result = env.parser.Parse(env);
    REQUIRE(parse_result);
    auto *expression = parse_result.GetFirst().get();
    CHECK(expression->GetLocation() == location);
    auto *function = llvm::dyn_cast<pink::Function>(expression);
    REQUIRE(function != nullptr);
    auto *block = llvm::dyn_cast<pink::Block>(function->GetBody().get());
    REQUIRE(block != nullptr);
    auto cursor = block->begin();
    REQUIRE(cursor != block->end());
    REQUIRE(llvm::dyn_cast<pink::Assignment>(cursor->get()));
  }
}