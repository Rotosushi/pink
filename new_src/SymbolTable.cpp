#include <optional>

#include "llvm/ADT/DenseMap.h"

#include "Ast.hpp"
#include "ScopeSet.hpp"
#include "StringInterner.hpp"
#include "SymbolTable.hpp"

SymbolTable::SymbolTable() :
  map(), outer(nullptr), scope(1)
{}

SymbolTable::SymbolTable(SymbolTable* outer)
{
  // map is default constructed
  this->outer = outer;
  // construct a new scope, relative to the outer scope;
  // containing all of the same scopes that have been built
  // up in the outer set. (because each scope we step into
  // adds another scope to the set, likewise each scope we
  // step out of we remove from the set.)
  this->scope = outer->scope.IntroduceNewScope(outer->scope);
}

std::optional<std::shared_ptr<Ast>> SymbolTable::LookupHelper(InternedString name, ScopeSet scope, std::shared_ptr<Ast> best_match, ScopeSet best_subset)
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
    // if we could search more, we will
    return outer->lookupHelper(name, scope, best_match, best_subset);
  }
  else if (best_match != nullptr)
  {
    // we have our best match, return it
    return std::optional<std::shared_ptr<Ast>>(best_match);
  }
  else
  {
    // we didin't find a match, return None
    return std::optional<std::shared_ptr<Ast>>();
  }
}

std::optional<Ast*> SymbolTable::Lookup(InternedString name, ScopeSet scope)
{
  return lookupHelper(name, scope, nullptr, ScopeSet());
}

std::optional<Ast*> SymbolTable::LookupLocal(InternedString name)
{
  auto itr = map.find(name);

  if (itr == map.end())
  {
    return std::optional<std::shared_ptr<Ast>>();
  }
  else
  {
    // as far as i can tell, dereferencing iterators
    // should always return a value type.
    return std::optional<std::shared_ptr<Ast>>(itr->getSecond());
  }
}

void SymbolTable::Bind(InternedString name, std::shared_ptr<Ast> value)
{
  auto result = map.try_emplace(name, value);

  if (result.second == false)
  {
    FatalError("Could no insert symbol into table.", __FILE__, __LINE__);
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
