#pragma once
#include <memory>
#include <optional>

#include "llvm/ADT/DenseMap.h"

#include "Ast.hpp"
#include "StringInterner.hpp"
#include "BinopCodegen.hpp"


class BinopTable
{
private:
  llvm::DenseMap<InternedString, std::shared_ptr<BinopCodegenTable>> map;

public:
  void RegisterBinop(InternedString operator, std::shared_ptr<BinopCodegenTable> eliminators);
  std::optional<std::shared_ptr<BinopCodegenTable>> Lookup(InternedString operator);
};
