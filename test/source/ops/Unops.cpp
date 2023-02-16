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

  pink::UnopCodegen unop_implementation(integer_type, UnopCodegenFunction);

  REQUIRE(unop_implementation.GetReturnType() == integer_type);
  REQUIRE(unop_implementation.GetGenerateFn() == UnopCodegenFunction);
}

TEST_CASE("ops/UnopLiteral", "[unit][ops]") {
  auto                type         = std::make_unique<pink::IntegerType>();
  pink::Type::Pointer integer_type = type.get();

  pink::UnopLiteral unop_literal;
  auto              implementation =
      unop_literal.Register(integer_type, integer_type, UnopCodegenFunction);
  REQUIRE(implementation != nullptr);
  REQUIRE(implementation->GetReturnType() == integer_type);
  REQUIRE(implementation->GetGenerateFn() == UnopCodegenFunction);

  auto optional_implementation = unop_literal.Lookup(integer_type);
  REQUIRE(optional_implementation.has_value());
  implementation = optional_implementation.value();
  REQUIRE(implementation != nullptr);
  REQUIRE(implementation->GetReturnType() == integer_type);
  REQUIRE(implementation->GetGenerateFn() == UnopCodegenFunction);
}

TEST_CASE("ops/UnopTable", "[unit][ops]") {
  pink::InternedString op           = "-";
  auto                 type         = std::make_unique<pink::IntegerType>();
  pink::Type::Pointer  integer_type = type.get();

  pink::UnopTable unop_table;
  auto            literal =
      unop_table.Register(op, integer_type, integer_type, UnopCodegenFunction);
  REQUIRE(literal != nullptr);
  auto optional_implementation = literal->Lookup(integer_type);
  REQUIRE(optional_implementation.has_value());
  auto implementation = optional_implementation.value();
  REQUIRE(implementation != nullptr);
  REQUIRE(implementation->GetReturnType() == integer_type);
  REQUIRE(implementation->GetGenerateFn() == UnopCodegenFunction);

  auto optional_literal = unop_table.Lookup(op);
  REQUIRE(optional_literal != nullptr);
  literal                 = optional_literal.value();
  optional_implementation = literal->Lookup(integer_type);
  REQUIRE(optional_implementation.has_value());
  implementation = optional_implementation.value();
  REQUIRE(implementation != nullptr);
  REQUIRE(implementation->GetReturnType() == integer_type);
  REQUIRE(implementation->GetGenerateFn() == UnopCodegenFunction);
}