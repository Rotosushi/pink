#include "runtime/SliceSubscript.h"
#include "runtime/LoadValue.h"
#include "runtime/RuntimeError.h"

namespace pink {
auto SliceSubscript(llvm::StructType *slice_type,
                    llvm::Type       *element_type,
                    llvm::Value      *slice,
                    llvm::Value      *index,
                    Environment      &env) -> llvm::Value * {
  assert(slice_type != nullptr);
  assert(element_type != nullptr);
  assert(slice != nullptr);
  assert(index != nullptr);
  // essentially:
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

  auto *slice_size_ptr =
      env.instruction_builder->CreateConstInBoundsGEP2_32(slice_type,
                                                          slice,
                                                          0,
                                                          0);
  auto *slice_offset_ptr =
      env.instruction_builder->CreateConstInBoundsGEP2_32(slice_type,
                                                          slice,
                                                          0,
                                                          1);
  auto *slice_ptr_ptr =
      env.instruction_builder->CreateConstInBoundsGEP2_32(slice_type,
                                                          slice,
                                                          0,
                                                          2);

  auto *slice_ptr =
      env.instruction_builder->CreateLoad(pointer_type, slice_ptr_ptr);
  auto *slice_size =
      env.instruction_builder->CreateLoad(integer_type, slice_size_ptr);
  auto *slice_offset =
      env.instruction_builder->CreateLoad(integer_type, slice_offset_ptr);
  // offset = slice.offset + index
  auto *offset = env.instruction_builder->CreateAdd(slice_offset, index);
  auto *zero   = env.instruction_builder->getInt64(0);

  auto *greater_than_bounds =
      env.instruction_builder->CreateICmpSLE(slice_size, offset);
  auto *less_than_zero = env.instruction_builder->CreateICmpSLT(offset, zero);
  auto *bounds_check =
      env.instruction_builder->CreateLogicalOr(greater_than_bounds,
                                               less_than_zero);

  auto *then_BB  = llvm::BasicBlock::Create(*env.context,
                                           "runtime_error",
                                           env.current_function);
  auto *after_BB = llvm::BasicBlock::Create(*env.context, "subscript");

  env.instruction_builder->CreateCondBr(bounds_check, then_BB, after_BB);

  // emit the runtime error into the true branch
  env.instruction_builder->SetInsertPoint(then_BB);

  /// \todo add a location to the runtime error description
  std::string errdsc = "index out of bounds\n";
  auto       *one    = env.instruction_builder->getInt64(1);
  RuntimeError(errdsc, one, env);
  env.instruction_builder->CreateBr(after_BB);

  // nv.current_function->getBasicBlockList().push_back(after_BB);
  env.current_function->insert(env.current_function->end(), after_BB);
  env.instruction_builder->SetInsertPoint(after_BB);

  auto *element = env.instruction_builder->CreateGEP(element_type,
                                                     slice_ptr,
                                                     {offset},
                                                     "subscript",
                                                     true);

  return LoadValue(element_type, element, env);
}
} // namespace pink