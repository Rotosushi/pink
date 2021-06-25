#include <memory>
#include <optional>

#include "llvm/IR/Type.h"
#include "llvm/ADT/DenseMap.h"

#include "Judgement.hpp"
#include "StringInterner.hpp"
#include "Ast.hpp"
#include "Object.hpp"

namespace pink {

UnopLiteral::UnopLiteral(llvm::Type* result_type, PrimitiveUnopCodegenFn codegen_function)
{
  this->result_type      = result_type;
  this->codegen_function = codegen_function;
}

Judgement UnopLiteral::Codegen(std::shared_ptr<Object> value, const Environment& env)
{
  // and here is where we would build an
  // application of a user defined procedure
  // that has been registered as a unop codegen
  // function. or, apply the primitive unop.
  return codegen_function(value, env);
}

UnopLiteralTable::UnopLiteralTable()
{

}

void UnopLiteralTable::RegisterUnop(llvm::Type* arg_type, llvm::Type* result_type, PrimitiveUnopCodegenFn codegen_function)
{
  std::pair<llvm::DenseMapIterator<llvm::Type*, std::shared_ptr<UnopLiteral>>, bool> mem = map.try_emplace(arg_type, std::make_shared<UnopLiteral>(result_type, codegen_function));
}

std::optional<std::shared_ptr<BinopCodegen>> UnopEliminatorTable::Lookup(llvm::Type* arg_type)
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

}
