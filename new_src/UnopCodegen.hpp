#pragma once
#include <memory>
#include <optional>


#include "llvm/IR/Type.h"
#include "llvm/ADT/DenseMap.h"

#include "Judgement.hpp"
#include "StringInterner.hpp"
#include "Ast.hpp"
#include "Object.hpp"

typedef Judgement (*PrimitiveUnopCodegenFn)(std::shared_ptr<Object> value, const Environment& env);

class UnopCodegen
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
  UnopEliminator(llvm::Type* result_type, PrimitiveUnopCodegenFn codegen_function);
  //UnopEliminator(llvm::Type* result_type, std::shared_ptr<Lambda> composite_codegen_function);

  Judgement Codegen(std::shared_ptr<Object> value, const Environment& env);
};

class UnopCodegenTable
{
private:
  llvm::DenseMap<llvm::Type*, std::shared_ptr<UnopCodegen>> map;

public:
  UnopCodegenTable();

  void RegisterUnopCodegen(llvm::Type* arg_type, llvm::Type* result_type, std::shared_ptr<UnopCodegen> eliminator);
  std::optional<std::shared_ptr<UnopEliminator>> Lookup(llvm::Type* arg_type);
};
