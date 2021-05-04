#include <memory>
#include <optional>
#include <map>
#include <utility>
#include <algorithm>

#include "llvm/IR/Type.h"

#include "Judgement.hpp"
#include "Ast.hpp"
#include "Object.hpp"

BinopEliminator::BinopEliminator(llvm::Type* result_type, PrimitiveBinopEliminator eliminator)
{
  this->result_type = result_type;
  this->eliminator  = eliminator;
}

Judgement BinopEliminator::Codegen(std::shared_ptr<Object> lhs_value, std::shared_ptr<Object> rhs_value, const Environment& env);
{
  return eliminator(lhs_value, rhs_value, env);
}

BinopArgs::BinopArgs(llvm::Type* lhs_type, llvm::Type* rhs_type)
{
  this->lhs_type = lhs_type;
  this->rhs_type = rhs_type;
}

bool BinopArgs::operator==(const BinopArgs& rhs)
{
  return ((this->lhs_type == rhs.lhs_type) && (this->rhs_type == rhs.rhs_type));
}

BinopEliminatorTable::BinopELiminatorTable()
{

}

/*
  maybe we want to take a second look at this with respect to
  being able to partially apply binops. (which implies being able to
  peel away arguments conceptually.)
*/
void BinopEliminatorTable::RegisterBinopCodegen(llvm::Type* lhs_type, llvm::Type* rhs_type, llvm::Type* result_type, std::shared_ptr<BinopEliminator> eliminator)
{
  table.emplace_back(std::make_pair(BinopArgs(lhs_type, rhs_type), std::make_shared(BinopEliminator(result_type, eliminator))));
}

std::optional<std::shared_ptr<BinopEliminator>> BinopCodegenTable::Lookup(llvm::Type* lhs_type, llvm::Type* rhs_type)
{
  std::vector<std::pair<BinopArgs, std::shared_ptr<BinopEliminator>>>::iterator itr = std::find(table.begin(), table.end(), BinopArgs(lhs_type, rhs_type));

  if (itr == table.end())
  {
    return std::optional<std::shared_ptr<BinopEliminator>>();
  }
  else
  {
    return std::optional<std::shared_ptr<BinopEliminator>>(itr->second());
  }
}
