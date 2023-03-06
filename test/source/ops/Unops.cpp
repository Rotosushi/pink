#include "catch2/catch_test_macros.hpp"

#include "ops/UnopTable.h"

#include "aux/Environment.h"

#include "support/DisableWarning.h"

auto UnopCodegenFunction([[maybe_unused]] llvm::Value       *term,
                         [[maybe_unused]] pink::Environment &env)
    -> llvm::Value * {
  return nullptr;
}

TEST_CASE("ops/UnopCodegen", "[unit][ops]") {
  auto        interner     = pink::TypeInterner{};
  const auto *integer_type = interner.GetIntType();

  pink::UnopCodegen implementation(integer_type, UnopCodegenFunction);

  REQUIRE(implementation.GetReturnType() == integer_type);
}

TEST_CASE("ops/ConcreteBuiltinUnopOverloadSet", "[unit][ops]") {
  auto        interner     = pink::TypeInterner{};
  const auto *integer_type = interner.GetIntType();

  pink::ConcreteBuiltinUnopOverloadSet unop_literal;
  unop_literal.Register(integer_type, integer_type, UnopCodegenFunction);

  auto optional_implementation = unop_literal.Lookup(integer_type);
  REQUIRE(optional_implementation.has_value());
  auto implementation = optional_implementation.value();
  REQUIRE(implementation.ReturnType() == integer_type);
}

TEST_CASE("ops/UnopTable", "[unit][ops]") {
  pink::InternedString op           = "-";
  auto                 interner     = pink::TypeInterner{};
  const auto          *integer_type = interner.GetIntType();

  pink::UnopTable unop_table;
  unop_table.RegisterBuiltin(op,
                             integer_type,
                             integer_type,
                             UnopCodegenFunction);

  auto optional_literal        = unop_table.Lookup(op);
  auto literal                 = optional_literal.value();
  auto optional_implementation = literal.Lookup(integer_type);
  REQUIRE(optional_implementation.has_value());
  auto implementation = optional_implementation.value();
  REQUIRE(implementation.ReturnType() == integer_type);
}