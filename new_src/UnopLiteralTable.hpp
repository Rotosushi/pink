#pragma once
#include <memory>
#include <optional>


#include "llvm/IR/Type.h"
#include "llvm/ADT/DenseMap.h"

#include "Judgement.hpp"
#include "StringInterner.hpp"
#include "Ast.hpp"

namespace pink {

typedef Judgement (*PrimitiveUnopCodegenFn)(std::shared_ptr<Ast>& value, const Environment& env);

class UnopLiteral
{
private:
  llvm::Type* result_type;
  PrimitiveUnopCodegenFn codegen_function;
  /*bool        is_primitive;
  union {
    PrimitiveUnopEliminator primitive_eliminator;
    std::shared_ptr<Lambda> composite_eliminator;
  };*/

public:
  UnopLiteral(llvm::Type* result_type, PrimitiveUnopCodegenFn codegen_function);
  //UnopEliminator(llvm::Type* result_type, std::shared_ptr<Lambda> composite_codegen_function);

  Judgement Codegen(std::shared_ptr<Object> value, const Environment& env);
};

class UnopLiteralTable
{
private:
  llvm::DenseMap<llvm::Type*, std::shared_ptr<UnopLiteral>> map;

public:
  UnopLiteralTable();

  void RegisterUnopLiteral(llvm::Type* arg_type, llvm::Type* result_type, PrimitiveUnopCodegenFn codegen_function);
  std::optional<std::shared_ptr<UnopLiteral>> Lookup(llvm::Type* arg_type);
};

}
