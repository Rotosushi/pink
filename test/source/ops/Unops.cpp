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
  auto              pair =
      unop_literal.Register(integer_type, integer_type, UnopCodegenFunction);
  REQUIRE(pair.first == integer_type);
  REQUIRE(pair.second != nullptr);
  REQUIRE(pair.second->GetReturnType() == integer_type);
  REQUIRE(pair.second->GetGenerateFn() == UnopCodegenFunction);

  auto found = unop_literal.Lookup(integer_type);
  REQUIRE(found.has_value());
  REQUIRE(found->first == integer_type);
  REQUIRE(found->second != nullptr);
  REQUIRE(found->second->GetReturnType() == integer_type);
  REQUIRE(found->second->GetGenerateFn() == UnopCodegenFunction);

  unop_literal.Unregister(integer_type);
  found = unop_literal.Lookup(integer_type);
  REQUIRE(!found.has_value());
}

TEST_CASE("ops/UnopTable", "[unit][ops]") {
  pink::InternedString op           = "-";
  auto                 type         = std::make_unique<pink::IntegerType>();
  pink::Type::Pointer  integer_type = type.get();

  pink::UnopTable unop_table;
  auto            pair =
      unop_table.Register(op, integer_type, integer_type, UnopCodegenFunction);
  REQUIRE(pair.first == op);
  REQUIRE(pair.second != nullptr);
  auto implementation = pair.second->Lookup(integer_type);
  REQUIRE(implementation.has_value());
  REQUIRE(implementation->first == integer_type);
  REQUIRE(implementation->second != nullptr);
  REQUIRE(implementation->second->GetReturnType() == integer_type);
  REQUIRE(implementation->second->GetGenerateFn() == UnopCodegenFunction);

  auto found = unop_table.Lookup(op);
  REQUIRE(found.has_value());
  REQUIRE(found->first == op);
  REQUIRE(found->second != nullptr);
  implementation = found->second->Lookup(integer_type);
  REQUIRE(implementation.has_value());
  REQUIRE(implementation->first == integer_type);
  REQUIRE(implementation->second != nullptr);
  REQUIRE(implementation->second->GetReturnType() == integer_type);
  REQUIRE(implementation->second->GetGenerateFn() == UnopCodegenFunction);

  unop_table.Unregister(op);
  found = unop_table.Lookup(op);
  REQUIRE(!found.has_value());
}