#pragma once
#include <memory>
#include <optional>

#include "llvm/ADT/DenseMap.h"

#include "Ast.hpp"
#include "StringInterner.hpp"
#include "UnopEliminatorTable.hpp"

class UnopTable
{
private:
  llvm::DenseMap<InternedString, std::shared_ptr<UnopCodegenTable>> map;
public:
  UnopTable();

  void RegisterUnop(InternedString operator, std::shared_ptr<UnopCodegenTable> eliminators);
  std::optional<std::shared_ptr<UnopCodegenTable>> Lookup(InternedString operator);
};
