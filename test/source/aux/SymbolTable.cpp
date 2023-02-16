#include "catch2/catch_test_macros.hpp"

#include "aux/SymbolTable.h"
#include "aux/TypeInterner.h"

TEST_CASE("aux/SymbolTable", "[unit][aux]") {
  pink::StringInterner string_interner;
  pink::TypeInterner   type_interner;
  pink::SymbolTable    scope;
  pink::SymbolTable    inner_scope(&scope);

  REQUIRE(scope.IsGlobal() == true);
  REQUIRE(scope.OuterScope() == nullptr);
  REQUIRE(inner_scope.IsGlobal() == false);
  REQUIRE(inner_scope.OuterScope() == &scope);

  const auto *variable_x = string_interner.Intern("x");
  const auto *type_x     = type_interner.GetBoolType();
  scope.Bind(variable_x, type_x, nullptr);

  const auto *variable_y = string_interner.Intern("y");
  const auto *type_y     = type_interner.GetIntType();
  inner_scope.Bind(variable_y, type_y, nullptr);

  // Resolve symbol in scope
  auto found_x = scope.Lookup(variable_x);
  REQUIRE(found_x.has_value());

  // Resolve symbol in outer_scope scope
  found_x = inner_scope.Lookup(variable_x);
  REQUIRE(found_x.has_value());

  // Don't resolve outer_scope symbol when searching local scope
  found_x = inner_scope.LookupLocal(variable_x);
  REQUIRE(!found_x.has_value());

  // Resolve local symobl when searching local scope
  auto found_y = inner_scope.LookupLocal(variable_y);
  REQUIRE(found_y.has_value());

  // Cannot resolve inner symbol from outer_scope scope
  found_y = scope.Lookup(variable_y);
  REQUIRE(!found_y.has_value());
}