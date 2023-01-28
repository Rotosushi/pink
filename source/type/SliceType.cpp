#include "type/SliceType.h"

#include "aux/Environment.h"

namespace pink {
/*
auto SliceType::EqualTo(Type *other) const -> bool {
  bool result = true;
  if (auto *other_slice = llvm::dyn_cast<SliceType>(other)) {
    if ((other_slice->pointee_type != pointee_type)) {
      result = false;
    }
    // else the types are equivalent.
  } else {
    result = false;
  }
  return result;
}

auto SliceType::ToString() const -> std::string {
  std::string result = "[]";
  result += pointee_type->ToString();
  return result;
}

// A slice is stored as a Tuple of (size, offset, pointer)
/// \todo slices don't need to be stored as {i64, i64, void*}
/// we could save space on slice allocation if we used smaller
/// integer types for the size and offset. The only issue is
/// handling the integer size discrepency between slices within
/// the runtime of the program itself
auto SliceType::ToLLVM(const Environment &env) const -> llvm::Type * {
  llvm::Type *integer_type = env.instruction_builder->getInt64Ty();
  auto *pointer_type = llvm::PointerType::getUnqual(*env.context);
  return llvm::StructType::get(
      *env.context, {integer_type, integer_type, pointer_type}, false);
}
*/
} // namespace pink