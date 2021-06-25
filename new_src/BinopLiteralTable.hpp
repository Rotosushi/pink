#pragma once
#include <memory>
#include <optional>
#include <map>
#include <utility>

#include "llvm/IR/Type.h"

#include "Judgement.hpp"
#include "Ast.hpp"

namespace pink {

typedef Judgement (*PrimitiveBinopCodegenFn)(std::shared_ptr<Ast>& rhs, std::shared_ptr<Ast>& lhs, const Environment& env);

class BinopLiteral
{
private:
  llvm::Type* result_type;
  PrimitiveBinopCodegenFn codegen_function;
  /*
  bool is_primitive;
  union {
    PrimitiveBinopEliminator primitive_eliminator;
    std::shared_ptr<Lambda>  composite_eliminator;
  };
  */

public:
  BinopLiteral(llvm::Type* result_type, PrimitiveBinopCodegenFn primitive_codegen_fn);
  // BinopCodegen(llvm::Type* result_type, std::shared_ptr<Lambda> composite_codegen_fn);

  llvm::Type* GetResultType();
  Judgement Codegen(std::shared_ptr<Object> lhs_value, std::shared_ptr<Object> rhs_value, const Environment& env);
};

class BinopArgs
{
private:
  llvm::Type* lhs_type;
  llvm::Type* rhs_type;

public:
  BinopArgs(llvm::Type* lhs_type, llvm::Type* rhs_type);

  bool operator==(const BinopArgs& rhs);
};

class BinopLiteralTable
{
private:
  std::vector<std::pair<BinopArgs, BinopLiteral>> table;

public:
  BinopLiteralTable();

  void RegisterBinopLiteral(llvm::Type* lhs_type, llvm::Type* rhs_type, llvm::Type* result_type, PrimitiveBinopCodegenFn primitive_codegen);
  std::optional<std::shared_ptr<BinopCodegen>> Lookup(llvm::Type* lhs_type, llvm::Type* rhs_type);

};

}
