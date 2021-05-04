#include <memory>
#include <optional>

#include "llvm/IR/Type.h"
#include "llvm/ADT/DenseMap.h"

#include "Judgement.hpp"
#include "StringInterner.hpp"
#include "Ast.hpp"
#include "Object.hpp"

UnopEliminator::UnopEliminator(llvm::Type* result_type, PrimitiveUnopEliminator eliminator)
{
  this->result_type = result_type;
  this->eliminator  = eliminator;
}

Judgement UnopEliminator::Codegen(std::shared_ptr<Object> value, const Environment& env)
{
  // call the eliminator held within, passing in the value.
  // this allows us to register new eliminators at runtime!
}

UnopEliminatorTable::UnopEliminatorTable()
{

}

void UnopEliminatorTable::RegisterUnop(llvm::Type* arg_type, llvm::Type* result_type, std::shared_ptr<UnopEliminator> eliminator)
{
  std::pair<llvm::DenseMapIterator<llvm::Type*, std::shared_ptr<BinopEliminator>>, bool> mem = map.try_emplace(arg_type, result_type, eliminator);
}

std::optional<std::shared_ptr<BinopEliminator>> UnopEliminatorTable::Lookup(llvm::Type* arg_type)
{
  llvm::DenseMapIterator<llvm::Type*, std::shared_ptr<BinopEliminator>> itr = map.find(arg_type);

  if (itr == map.end())
  {
    return std::optional<std::shared_ptr<BinopEliminator>>();
  }
  else
  {
    return std::optional<std::shared_ptr<BinopEliminator>>(*itr);
  }
}
