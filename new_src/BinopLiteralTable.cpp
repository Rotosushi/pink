#include <memory>
#include <optional>
#include <map>
#include <utility>
#include <algorithm>

#include "llvm/IR/Type.h"

#include "Judgement.hpp"
#include "Ast.hpp"
#include "Object.hpp"
#include "BinopLiteralTable.hpp"

namespace pink {

BinopLiteral::BinopLiteral(llvm::Type* result_type, PrimitiveBinopCodegenFn codegen_function)
{
  this->result_type      = result_type;
  this->codegen_function = codegen_function;
}

llvm::Type* BinopLiteral::GetResultType()
{
  return result_type;
}

Judgement BinopLiteral::Codegen(std::shared_ptr<Object> lhs_value, std::shared_ptr<Object> rhs_value, const Environment& env);
{
  return codegen_function(lhs_value, rhs_value, env);
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

BinopLiteralTable::BinopLiteralTable()
{

}

/*
  maybe we want to take a second look at this with respect to
  being able to partially apply binops. (which implies being able to
  peel away arguments conceptually.)
*/
void BinopLiteralTable::RegisterBinopLiteral(llvm::Type* lhs_type, llvm::Type* rhs_type, llvm::Type* result_type, PrimitiveBinopCodegenFn codegen_function)
{
  table.emplace_back(std::make_pair(BinopArgs(lhs_type, rhs_type), std::make_shared(BinopLiteral(result_type, codegen_function))));
}

std::optional<std::shared_ptr<BinopLiteral>> BinopCodegenTable::Lookup(llvm::Type* lhs_type, llvm::Type* rhs_type)
{
  std::vector<std::pair<BinopArgs, std::shared_ptr<BinopLiteral>>>::iterator itr = std::find(table.begin(), table.end(), BinopArgs(lhs_type, rhs_type));

  if (itr == table.end())
  {
    return std::optional<std::shared_ptr<BinopLiteral>>();
  }
  else
  {
    return std::optional<std::shared_ptr<BinopLiteral>>(itr->second());
  }
}

}
