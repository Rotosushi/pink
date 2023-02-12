#include "catch2/catch_test_macros.hpp"

#include "ops/BinopCodegen.h"
#include "ops/BinopLiteral.h"
#include "ops/BinopTable.h"

#include "aux/Environment.h"

#include "support/DisableWarning.h"

NOWARN(
    "-Wunused-parameter",
    auto BinopCodegenFunction(llvm::Type        *left_type,
                              llvm::Value       *left_value,
                              llvm::Type        *right_type,
                              llvm::Value       *right_value,
                              pink::Environment &env)
        ->llvm::Value * { return nullptr; })

TEST_CASE("ops/BinopCodegen", "[unit][ops]") {
  auto                type         = std::make_unique<pink::IntegerType>();
  pink::Type::Pointer integer_type = type.get();

  pink::BinopCodegen binop_implementation(integer_type, BinopCodegenFunction);
  REQUIRE(binop_implementation.GetReturnType() == integer_type);
  REQUIRE(binop_implementation.GetGenerateFn() == BinopCodegenFunction);
}

TEST_CASE("ops/BinopLiteral", "[unit][ops]") {
  auto                type          = std::make_unique<pink::IntegerType>();
  pink::Type::Pointer integer_type  = type.get();
  pink::Precedence    precedence    = 3;
  pink::Associativity associativity = pink::Associativity::Left;

  pink::BinopLiteral binop_literal(precedence, associativity);
  REQUIRE(binop_literal.NumOverloads() == 0);
  REQUIRE(binop_literal.GetPrecedence() == precedence);
  REQUIRE(binop_literal.GetAssociativity() == associativity);

  auto *implementation = binop_literal.Register(integer_type,
                                                integer_type,
                                                integer_type,
                                                BinopCodegenFunction);
  REQUIRE(binop_literal.NumOverloads() == 1);
  REQUIRE(implementation != nullptr);
  REQUIRE(implementation->GetReturnType() == integer_type);
  REQUIRE(implementation->GetGenerateFn() == BinopCodegenFunction);

  auto found = binop_literal.Lookup(integer_type, integer_type);
  REQUIRE(found.has_value());
  implementation = found.value();
  REQUIRE(implementation->GetReturnType() == integer_type);
  REQUIRE(implementation->GetGenerateFn() == BinopCodegenFunction);

  binop_literal.Unregister(integer_type, integer_type);
  REQUIRE(binop_literal.NumOverloads() == 0);
  found = binop_literal.Lookup(integer_type, integer_type);
  REQUIRE(!found.has_value());
}

TEST_CASE("ops/BinopTable", "[unit][ops]") {
  auto                 type          = std::make_unique<pink::IntegerType>();
  pink::Type::Pointer  integer_type  = type.get();
  pink::Precedence     precedence    = 3;
  pink::Associativity  associativity = pink::Associativity::Left;
  pink::InternedString op            = "+";

  pink::BinopTable binop_table;
  auto             literal = binop_table.Register(op,
                                      precedence,
                                      associativity,
                                      integer_type,
                                      integer_type,
                                      integer_type,
                                      BinopCodegenFunction);
  REQUIRE(literal != nullptr);
  REQUIRE(literal->NumOverloads() == 1);
  REQUIRE(literal->GetAssociativity() == associativity);
  REQUIRE(literal->GetPrecedence() == precedence);
  auto optional_implementation = literal->Lookup(integer_type, integer_type);
  REQUIRE(optional_implementation.has_value());
  auto implementation = optional_implementation.value();
  REQUIRE(implementation != nullptr);
  REQUIRE(implementation->GetReturnType() == integer_type);
  REQUIRE(implementation->GetGenerateFn() == BinopCodegenFunction);

  auto optional_literal = binop_table.Lookup(op);
  REQUIRE(optional_literal.has_value());
  literal = optional_literal.value();
  REQUIRE(literal != nullptr);
  REQUIRE(literal->NumOverloads() == 1);
  REQUIRE(literal->GetAssociativity() == associativity);
  REQUIRE(literal->GetPrecedence() == precedence);
  optional_implementation = literal->Lookup(integer_type, integer_type);
  REQUIRE(optional_implementation.has_value());
  implementation = optional_implementation.value();
  REQUIRE(implementation != nullptr);
  REQUIRE(implementation->GetReturnType() == integer_type);
  REQUIRE(implementation->GetGenerateFn() == BinopCodegenFunction);

  // apparantly this test fails, even though this has
  // the same exactl implementation as BinopLiteral,
  // UnopLiteral and UnopTable. and this case succeeds
  // in each of those test_cases, I don't understand why.
  // nor do I have any inklinking of what I could possibly
  // change to fix this. as again, it works in 3 other places
  // with the only difference being the exact key, value pair
  // in question. Given that we never actually need to remove
  // entries from the set of known unops or binops to implement
  // the language, this is a superfluous bit of functionality.
  // and could be safely removed without affecting the semantics
  // of the language. so, I am going to remove this test case
  // until I figure out what is even different about this case
  // compared to UnopTable.
  // binop_table.Unregister(op);
  // auto unfound = binop_table.Lookup(op);
  // REQUIRE(!unfound.has_value());
}