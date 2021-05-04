#pragma once
#include <memory>
#include <optional>

#include "llvm/ADT/DenseMap.h"

#include "Ast.hpp"
#include "StringInterner.hpp"
#include "UnopEliminatorTable.hpp"
#include "UnopTable.h"

UnopTable::UnopTable()
{

}

void UnopTable::RegisterUnop(InternedString operator, std::shared_ptr<UnopEliminatorTable> eliminators)
{
  auto res = map.try_emplace(operator, eliminators);
}

std::optional<std::shared_ptr<UnopEliminatorTable>> UnopTable::Lookup(InternedString operator)
{
  llvm::DenseMapIterator<InternedString, std::shared_ptr<UnopEliminatorTable>> itr = map.find(operator);

  if (itr == map.end())
  {
    return std::optional<std::shared_ptr<UnopEliminatorTable>>();
  }
  else
  {
    return std::optional<std::shared_ptr<UnopEliminatorTable>>(*itr);
  }
}
