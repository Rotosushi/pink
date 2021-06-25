#include <memory>
#include <optional>

#include "llvm/ADT/DenseMap.h"

#include "Ast.hpp"
#include "StringInterner.hpp"
#include "BinopEliminators.hpp"
#include "BinopTable.hpp"

namespace pink {

void BinopTable::RegisterBinop(InternedString operator, std::shared_ptr<BinopEliminatorTable> eliminators)
{
  map.emplace_back(operator, eliminators);
}

std::optional<std::shared_ptr<BinopEliminator>> BinopTable::Lookup(InternedString operator)
{
  llvm::DenseMapIterator<InternedString, std::shared_ptr<BinopEliminatorTable>> itr = map.find(operator);

  if (itr == map.end())
  {
    return std::optional<std::shared_ptr<BinopEliminators>>();
  }
  else
  {
    return std::optional<std::shared_ptr<BinopEliminators>>(*itr);
  }
}

}
