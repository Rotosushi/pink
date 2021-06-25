#include <optional>

#include "llvm/ADT/DenseMap.h"

#include "Ast.hpp"
#include "ScopeSet.hpp"
#include "StringInterner.hpp"
#include "SymbolTable.hpp"

namespace pink {

SymbolTable::SymbolTable()
  : map(), outer(nullptr), scope(1), tmpcnt(0)
{

}

SymbolTable::SymbolTable(SymbolTable* outer)
  : map(), outer(outer), scope(outer->scope.IntroduceNewScope(outer->scope)), tmpcnt(0)
{

}

bool SymbolTable::IsGlobalScope()
{
  return outer == nullptr;
}

ScopeSet SymbolTable::GetScope()
{
  return scope;
}

SymbolTable* SymbolTable::GetParentSymbolTable()
{
  return outer;
}

std::optional<llvm::AllocaInst*> SymbolTable::LookupHelper(InternedString name, ScopeSet scope, llvm::AllocaInst* best_match, ScopeSet best_subset)
{
  if (best_match == nullptr)
  {
    // search against no current best match
    auto itr = map.find(name);

    if (itr != map.end())
    {
      // the binding we just found is now the current best
      best_match  = (itr->getSecond());
      best_subset = this->scope.Subset(scope);
    }
  }
  else
  {
    // search against the current best match
    auto itr = map.find(name);

    if (itr != map.end())
    {
      //
      ScopeSet subset = this->scope.Subset(scope);
      if (best_subset < subset)
      {
        // this symbol is bound in a greater scope.
        // and so this is a better match for the symbol
        // we are looking for.
        best_match  = (itr->getSecond());
        best_subset = subset;
      }
    }
  }

  if (outer != nullptr)
  {
    // if we could search more, we do
    return outer->lookupHelper(name, scope, best_match, best_subset);
  }
  else if (best_match != nullptr)
  {
    // we have our best match, return it
    return std::optional<llvm::AllocaInst*>(best_match);
  }
  else
  {
    // we didin't find a match, return None
    return std::optional<std::shared_ptr<Ast>>();
  }
}

std::optional<llvm::AllocaInst*> SymbolTable::Lookup(InternedString name, ScopeSet scope)
{
  return lookupHelper(name, scope, nullptr, ScopeSet());
}

std::optional<llvm::AllocaInst*> SymbolTable::LookupLocal(InternedString name)
{
  auto itr = map.find(name);

  if (itr == map.end())
  {
    return std::optional<llvm::AllocaInst*>();
  }
  else
  {
    return std::optional<llvm::AllocaInst*>(itr->getSecond());
  }
}

void SymbolTable::Bind(InternedString name, std::shared_ptr<Ast> value)
{
  auto result = map.try_emplace(name, value);

  if (result.second == false)
  {
    FatalError("Could not insert symbol into table.", __FILE__, __LINE__);
  }
}

void SymbolTable::Unbind(InternedString name)
{
  bool result = map.erase(name);

  if (result == false)
  {
    FatalError("Could not erase name from table.", __FILE__, __LINE__);
  }
}

}
