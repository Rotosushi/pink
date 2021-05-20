#pragma once
#include <memory>
#include <optional>

#include "llvm/ADT/DenseMap.h"

#include "Ast.hpp"
#include "StringInterner.hpp"
#include "UnopLiteralTable.hpp"
#include "UnopTable.h"

UnopTable::UnopTable()
{

}

void UnopTable::RegisterUnop(InternedString operator, std::shared_ptr<UnopLiteralTable> eliminators)
{
  auto res = map.try_emplace(operator, eliminators);
}

std::optional<std::shared_ptr<UnopLiteralTable>> UnopTable::Lookup(InternedString operator)
{
  llvm::DenseMapIterator<InternedString, std::shared_ptr<UnopLiteralTable>> itr = map.find(operator);

  if (itr == map.end())
  {
    return std::optional<std::shared_ptr<UnopLiteralTable>>();
  }
  else
  {
    return std::optional<std::shared_ptr<UnopLiteralTable>>(*itr);
  }
}
