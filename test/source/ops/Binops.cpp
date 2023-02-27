#include "catch2/catch_test_macros.hpp"

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
  REQUIRE(binop_implementation.ReturnType() == integer_type);
  REQUIRE(binop_implementation.Generate() == BinopCodegenFunction);
}

TEST_CASE("ops/BinopOverloadSet", "[unit][ops]") {
  auto                type          = std::make_unique<pink::IntegerType>();
  pink::Type::Pointer integer_type  = type.get();
  pink::Precedence    precedence    = 3;
  pink::Associativity associativity = pink::Associativity::Left;

  pink::BinopOverloadSet binop_literal(precedence, associativity);
  REQUIRE(binop_literal.Empty());
  REQUIRE(binop_literal.Precedence() == precedence);
  REQUIRE(binop_literal.Associativity() == associativity);

  auto implementation = binop_literal.Register(integer_type,
                                               integer_type,
                                               integer_type,
                                               BinopCodegenFunction);
  REQUIRE(!binop_literal.Empty());
  REQUIRE(implementation.ReturnType() == integer_type);
  REQUIRE(implementation.Generate() == BinopCodegenFunction);

  auto found = binop_literal.Lookup(integer_type, integer_type);
  REQUIRE(found.has_value());
  implementation = found.value();
  REQUIRE(implementation.ReturnType() == integer_type);
  REQUIRE(implementation.Generate() == BinopCodegenFunction);
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
  REQUIRE(!literal.Empty());
  REQUIRE(literal.Associativity() == associativity);
  REQUIRE(literal.Precedence() == precedence);
  auto optional_implementation = literal.Lookup(integer_type, integer_type);
  REQUIRE(optional_implementation.has_value());
  auto implementation = optional_implementation.value();
  REQUIRE(implementation.ReturnType() == integer_type);
  REQUIRE(implementation.Generate() == BinopCodegenFunction);

  auto optional_literal = binop_table.Lookup(op);
  REQUIRE(optional_literal.has_value());
  literal = optional_literal.value();
  REQUIRE(!literal.Empty());
  REQUIRE(literal.Associativity() == associativity);
  REQUIRE(literal.Precedence() == precedence);
  optional_implementation = literal.Lookup(integer_type, integer_type);
  REQUIRE(optional_implementation.has_value());
  implementation = optional_implementation.value();
  REQUIRE(implementation.ReturnType() == integer_type);
  REQUIRE(implementation.Generate() == BinopCodegenFunction);
}