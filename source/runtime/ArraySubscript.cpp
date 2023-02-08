#include "runtime/ArraySubscript.h"
#include "runtime/LoadValue.h"
#include "runtime/RuntimeError.h"

namespace pink {
auto ArraySubscript(llvm::StructType *array_type,
                    llvm::Type       *element_type,
                    llvm::Value      *array,
                    llvm::Value      *index,
                    Environment      &env) -> llvm::Value * {
  assert(array_type != nullptr);
  assert(element_type != nullptr);
  assert(array != nullptr);
  assert(index != nullptr);

  auto *integer_type = env.instruction_builder->getInt64Ty();

  auto *array_size_ptr =
      env.instruction_builder->CreateConstInBoundsGEP2_32(array_type,
                                                          array,
                                                          0,
                                                          0);
  auto *array_ptr =
      env.instruction_builder->CreateConstInBoundsGEP2_32(array_type,
                                                          array,
                                                          0,
                                                          1);

  auto *zero = env.instruction_builder->getInt64(0);
  auto *array_size =
      env.instruction_builder->CreateLoad(integer_type, array_size_ptr);
  auto *greater_than_bounds =
      env.instruction_builder->CreateICmpSLE(array_size, index);
  auto *less_than_zero = env.instruction_builder->CreateICmpSLT(index, zero);
  // bounds_check = (array.size <= index) || (index < 0)
  auto *bounds_check =
      env.instruction_builder->CreateLogicalOr(greater_than_bounds,
                                               less_than_zero);

  llvm::BasicBlock *then_BB =
      llvm::BasicBlock::Create(*env.context, "then", env.current_function);
  llvm::BasicBlock *after_BB = llvm::BasicBlock::Create(*env.context, "after");

  env.instruction_builder->CreateCondBr(bounds_check, then_BB, after_BB);
  env.instruction_builder->SetInsertPoint(then_BB);

  std::string errdsc = "index out of bounds\n";
  auto       *one    = env.instruction_builder->getInt64(1);
  RuntimeError(errdsc, one, env);
  env.instruction_builder->CreateBr(after_BB);

  env.current_function->insert(env.current_function->end(), after_BB);
  env.instruction_builder->SetInsertPoint(after_BB);

  auto *element = env.instruction_builder->CreateGEP(element_type,
                                                     array_ptr,
                                                     {index},
                                                     "subscript",
                                                     true);

  return LoadValue(element_type, element, env);
}
} // namespace pink