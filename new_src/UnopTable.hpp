#pragma once
#include <memory>
#include <optional>

#include "llvm/ADT/DenseMap.h"

#include "Ast.hpp"
#include "StringInterner.hpp"
#include "UnopEliminatorTable.hpp"

namespace pink {

class UnopTable
{
private:
  llvm::DenseMap<InternedString, std::shared_ptr<UnopLiteralTable>> map;
public:
  UnopTable();

  void RegisterUnop(InternedString operator, std::shared_ptr<UnopLiteralTable> eliminators);
  std::optional<std::shared_ptr<UnopLiteralTable>> Lookup(InternedString operator);
};

}
