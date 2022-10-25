#include "kernel/SliceSubscript.h"
#include "kernel/LoadValue.h"
#include "kernel/RuntimeError.h"

namespace pink {
auto SliceSubscript(llvm::StructType *slice_type, llvm::Type *element_type,
                    llvm::Value *slice, llvm::Value *index,
                    const Environment &env) -> llvm::Value * {
  // in order to properly check array access through a slice,
  // from the perspective of forwards iteration, all we need
  // to know is the size of the rest of the array which the
  // slice points too. And that works if all we are expecting
  // is positive indicies. However, if we are given a negative
  // index (which is reasonable given the random access nature of
  // array pointers) then we must know how far we can go in the
  // reverse direction. This is fairly simple, however it requires
  // a modification to the slice structure itself. We must store
  // the size of the entire array and the offset which the slice
  // points too. Then, bounds checking the first is a simple matter
  // of seeing if the offset plus the index is greater than the
  // bounds, or if the offset minus the index is greater than zero.
  // which we can note is only possible if the index is a negative
  // value. so, we must make a choice here, is the defualt integer
  // type signed or unsigned? and how do we transmit that information
  // to this method?
  // if (bounds <= (index + offset) || (index + offset) < 0)
  //    RuntimeError("index out of bounds");
  //
  // auto *element_ptr = GEP(element_type, slice_ptr, index);
  // if (isSingleValueTy(element_type))
  //   return load(elice_element_type, element_ptr);
  // else
  //   return element_ptr;
  auto *integer_type = env.instruction_builder->getInt64Ty();
  auto *pointer_type = env.instruction_builder->getPtrTy();

  auto *slice_size_ptr = env.instruction_builder->CreateConstInBoundsGEP2_32(
      slice_type, slice, 0, 0);
  auto *slice_offset_ptr = env.instruction_builder->CreateConstInBoundsGEP2_32(
      slice_type, slice, 0, 1);
  auto *slice_ptr_ptr = env.instruction_builder->CreateConstInBoundsGEP2_32(
      slice_type, slice, 0, 2);

  auto *slice_ptr =
      env.instruction_builder->CreateLoad(pointer_type, slice_ptr_ptr);
  auto *slice_size =
      env.instruction_builder->CreateLoad(integer_type, slice_size_ptr);
  auto *slice_offset =
      env.instruction_builder->CreateLoad(integer_type, slice_offset_ptr);
  // offset = slice.offset + index
  auto *offset = env.instruction_builder->CreateAdd(slice_offset, index);
  auto *zero = env.instruction_builder->getInt64(0);

  auto *greater_than_bounds =
      env.instruction_builder->CreateICmpSLE(slice_size, offset);
  auto *less_than_zero = env.instruction_builder->CreateICmpSLT(offset, zero);
  auto *bounds_check = env.instruction_builder->CreateLogicalOr(
      greater_than_bounds, less_than_zero);

  llvm::BasicBlock *then_BB =
      llvm::BasicBlock::Create(*env.context, "then", env.current_function);
  llvm::BasicBlock *after_BB = llvm::BasicBlock::Create(*env.context, "after");

  env.instruction_builder->CreateCondBr(bounds_check, then_BB, after_BB);

  // emit the runtime error into the true branch
  auto then_insertion_point = then_BB->getFirstInsertionPt();
  auto then_builder =
      std::make_shared<llvm::IRBuilder<>>(then_BB, then_insertion_point);
  Environment then_env(env, then_builder);
  /// \todo add a location to the runtime error description
  std::string errdsc = "index out of bounds";
  auto *one = env.instruction_builder->getInt64(1);
  RuntimeError(errdsc, one, then_env);
  // this branch will not be reached, however it might need
  // to exist for the basic block to be considered complete
  // by llvm.
  then_env.instruction_builder->CreateBr(after_BB);

  // emit the array subscript into the false branch
  env.current_function->getBasicBlockList().push_back(after_BB);
  // ensure the rest of the code is generated after the subscript operation
  env.instruction_builder->SetInsertPoint(after_BB);

  auto *element = env.instruction_builder->CreateGEP(
      element_type, slice_ptr, {offset}, "subscript", true);

  return LoadValue(element_type, element, env);
}
} // namespace pink