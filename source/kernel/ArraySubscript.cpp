#include "kernel/ArraySubscript.h"
#include "kernel/LoadValue.h"
#include "kernel/RuntimeError.h"

namespace pink {
auto ArraySubscript(llvm::StructType *array_type, llvm::Type *element_type,
                    llvm::Value *array, llvm::Value *index,
                    const Environment &env) -> llvm::Value * {
  auto *integer_type = env.instruction_builder->getInt64Ty();

  auto *array_size_ptr = env.instruction_builder->CreateConstInBoundsGEP2_32(
      array_type, array, 0, 0);
  auto *array_ptr = env.instruction_builder->CreateConstInBoundsGEP2_32(
      array_type, array, 0, 1);

  auto *zero = env.instruction_builder->getInt64(0);
  auto *array_size =
      env.instruction_builder->CreateLoad(integer_type, array_size_ptr);
  auto *greater_than_bounds =
      env.instruction_builder->CreateICmpSLE(array_size, index);
  auto *less_than_zero = env.instruction_builder->CreateICmpSLT(index, zero);
  // bounds_check = (array.size <= index) || (index < 0)
  auto *bounds_check = env.instruction_builder->CreateLogicalOr(
      greater_than_bounds, less_than_zero);

  llvm::BasicBlock *then_BB =
      llvm::BasicBlock::Create(*env.context, "then", env.current_function);
  llvm::BasicBlock *after_BB = llvm::BasicBlock::Create(*env.context, "after");

  env.instruction_builder->CreateCondBr(bounds_check, then_BB, after_BB);

  auto then_insertion_point = then_BB->getFirstInsertionPt();
  auto then_builder =
      std::make_shared<llvm::IRBuilder<>>(then_BB, then_insertion_point);
  auto then_env = Environment::NewLocalEnv(env, then_builder);

  std::string errdsc = "index out of bounds\n";
  auto *one = env.instruction_builder->getInt64(1);
  RuntimeError(errdsc, one, *then_env);
  then_env->instruction_builder->CreateBr(after_BB);

  env.current_function->getBasicBlockList().push_back(after_BB);

  env.instruction_builder->SetInsertPoint(after_BB);

  auto *element = env.instruction_builder->CreateGEP(
      element_type, array_ptr, {index}, "subscript", true);

  return LoadValue(element_type, element, env);
}
} // namespace pink