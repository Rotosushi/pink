#include "aux/TestSymbolTable.h"
#include "Test.h"
#include "aux/SymbolTable.h"

#include "aux/Environment.h"

#include "ast/Nil.h"

auto TestSymbolTable(std::ostream &out) -> bool {
  bool result = true;
  out << "\n-----------------------\n";
  out << "Testing pink::SymbolTable: \n";

  auto options = std::make_shared<pink::CLIOptions>();
  auto env = pink::Environment::NewGlobalEnv(options);

  pink::SymbolTable symbol_table(env->bindings.get());

  result &= Test(out, "SymbolTable::OuterScope(), global scope",
                 env->bindings->OuterScope() == nullptr);

  result &= Test(out, "SymbolTable::OuterScope(), inner scope",
                 symbol_table.OuterScope() == env->bindings.get());

  llvm::Value *nil = env->instruction_builder->getFalse();
  pink::Type *nil_t = env->types->GetNilType();
  pink::InternedString symbol_x = env->symbols->Intern("x");

  env->bindings->Bind(symbol_x, nil_t, nil);
  auto binding_0 = env->bindings->Lookup(symbol_x);

  result &= Test(out, "SymbolTable::Bind()", binding_0.has_value());
  result &= Test(out, "SymbolTable::Lookup(), symbol bound in local scope",
                 (*binding_0).first == nil_t && (*binding_0).second == nil);

  auto binding_1 = symbol_table.Lookup(symbol_x);
  result &= Test(out, "SymbolTable::Lookup(), symbol bound in outer scope",
                 binding_1.has_value() && binding_1->first == nil_t &&
                     binding_1->second == nil);

  auto local_binding = symbol_table.LookupLocal(symbol_x);
  result &= Test(out, "SymbolTable::LookupLocal(), variable not in local scope",
                 !local_binding.has_value());

  local_binding = env->bindings->LookupLocal(symbol_x);
  result &= Test(out, "SymbolTable::LookupLocal(), variable in local scope",
                 local_binding.has_value() && local_binding->first == nil_t &&
                     local_binding->second == nil);

  env->bindings->Unbind(symbol_x);
  auto unbound_symbol = env->bindings->Lookup(symbol_x);

  result &= Test(out, "SymbolTable::Unbind()", !unbound_symbol.has_value());

  result &= Test(out, "pink::SymbolTable", result);
  out << "\n-----------------------\n";
  return result;
}
