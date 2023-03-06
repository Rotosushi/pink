#include "catch2/catch_test_macros.hpp"

#include "aux/ScopeStack.h"
#include "type/interner/TypeInterner.h"

TEST_CASE("aux/SymbolTable", "[unit][aux]") {
  pink::StringInterner string_interner;
  pink::TypeInterner   type_interner;
  pink::ScopeStack     scopes;

  REQUIRE(scopes.IsGlobal() == true);

  const auto *variable_x = string_interner.Intern("x");
  const auto *type_x     = type_interner.GetBoolType();
  scopes.Bind(variable_x, type_x, nullptr);

  scopes.PushScope();

  const auto *variable_y = string_interner.Intern("y");
  const auto *type_y     = type_interner.GetIntType();
  scopes.Bind(variable_y, type_y, nullptr);

  // Resolve symbol in scope
  auto found_x = scopes.Lookup(variable_x);
  REQUIRE(found_x);

  // Don't resolve outer scope symbol when searching local scope
  found_x = scopes.LookupLocal(variable_x);
  REQUIRE(!found_x);

  // Resolve local symbol when searching local scope
  auto found_y = scopes.LookupLocal(variable_y);
  REQUIRE(found_y);

  // Resolve local symbol through normal lookup
  found_y = scopes.Lookup(variable_y);
  REQUIRE(found_y);

  scopes.PopScope();

  // Cannot resolve inner symbol from outer_scope scope
  found_y = scopes.Lookup(variable_y);
  REQUIRE(!found_y);

  // Can still resolve outer symbol after poping
  found_x = scopes.Lookup(variable_x);
  REQUIRE(found_x);

  found_x = scopes.LookupLocal(variable_x);
  REQUIRE(found_x);
}