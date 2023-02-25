#include "catch2/catch_test_macros.hpp"

#include "ops/UnopCodegen.h"
#include "ops/UnopLiteral.h"
#include "ops/UnopTable.h"

#include "aux/Environment.h"

#include "support/DisableWarning.h"

NOWARN(
    "-Wunused-parameter",
    auto UnopCodegenFunction(llvm::Value *term, pink::Environment &env)
        ->llvm::Value * { return nullptr; })

TEST_CASE("ops/UnopCodegen", "[unit][ops]") {
  auto                type         = std::make_unique<pink::IntegerType>();
  pink::Type::Pointer integer_type = type.get();

  pink::UnopCodegen implementation(integer_type, UnopCodegenFunction);

  REQUIRE(implementation.GetReturnType() == integer_type);
  REQUIRE(implementation.GetFunction() == UnopCodegenFunction);
}

TEST_CASE("ops/UnopLiteral", "[unit][ops]") {
  auto                type         = std::make_unique<pink::IntegerType>();
  pink::Type::Pointer integer_type = type.get();

  pink::UnopLiteral unop_literal;
  unop_literal.Register(integer_type,
                        pink::UnopCodegen{integer_type, UnopCodegenFunction});

  auto optional_implementation = unop_literal.Lookup(integer_type);
  REQUIRE(optional_implementation.has_value());
  auto *implementation = optional_implementation.value();
  REQUIRE(implementation != nullptr);
  REQUIRE(implementation->second.GetReturnType() == integer_type);
  REQUIRE(implementation->second.GetFunction() == UnopCodegenFunction);
}

TEST_CASE("ops/UnopTable", "[unit][ops]") {
  pink::InternedString op           = "-";
  auto                 type         = std::make_unique<pink::IntegerType>();
  pink::Type::Pointer  integer_type = type.get();

  pink::UnopLiteral unop_literal;
  unop_literal.Register(integer_type,
                        pink::UnopCodegen(integer_type, UnopCodegenFunction));

  pink::UnopTable unop_table;
  unop_table.Register(op, std::move(unop_literal));

  auto optional_literal = unop_table.Lookup(op);
  REQUIRE(optional_literal != nullptr);
  auto *literal                 = optional_literal.value();
  auto  optional_implementation = literal->second.Lookup(integer_type);
  REQUIRE(optional_implementation.has_value());
  auto *implementation = optional_implementation.value();
  REQUIRE(implementation != nullptr);
  REQUIRE(implementation->second.GetReturnType() == integer_type);
  REQUIRE(implementation->second.GetFunction() == UnopCodegenFunction);
}