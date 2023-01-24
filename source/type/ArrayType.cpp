#include "type/ArrayType.h"

#include "aux/Environment.h"

#include "llvm/IR/DerivedTypes.h"

namespace pink {
/*
auto ArrayType::EqualTo(Type *other) const -> bool {
  bool equal = true;

  auto *array_type = llvm::dyn_cast<ArrayType>(other);
  if (array_type != nullptr) {
    if ((array_type->size != size) ||
        (array_type->element_type != member_type)) {
      equal = false;
    }
  } else {
    equal = false;
  }

  return equal;
}
*/
/*
auto ArrayType::ToString() const -> std::string {
  std::string result;

  result += "[";
  result += member_type->ToString();
  result += " x ";
  result += std::to_string(size);
  result += "]";

  return result;
}

auto ArrayType::ToLLVM(const Environment &env) const -> llvm::Type * {
  auto *llvm_array_type = llvm::ArrayType::get(member_type->ToLLVM(env), size);
  auto *llvm_integer_type = env.types->GetIntType()->ToLLVM(env);
  return llvm::StructType::get(*env.context,
                               {llvm_integer_type, llvm_array_type});
}
*/
} // namespace pink
