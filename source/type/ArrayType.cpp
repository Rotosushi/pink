#include "type/ArrayType.h"

#include "aux/Environment.h"

#include "llvm/IR/DerivedTypes.h"

namespace pink {
ArrayType::ArrayType(size_t size, Type *member_type)
    : Type(Type::Kind::Array), size(size), member_type(member_type) {}

auto ArrayType::classof(const Type *type) -> bool {
  return type->GetKind() == Type::Kind::Array;
}

auto ArrayType::EqualTo(Type *other) const -> bool {
  bool equal = true;

  auto *array_type = llvm::dyn_cast<ArrayType>(other);
  if (array_type != nullptr) {
    if ((array_type->size != size) ||
        (array_type->member_type != member_type)) {
      equal = false;
    }
  } else {
    equal = false;
  }

  return equal;
}

auto ArrayType::ToString() const -> std::string {
  std::string result;

  result += "[";
  result += member_type->ToString();
  result += " x ";
  result += std::to_string(size);
  result += "]";

  return result;
}

auto ArrayType::Codegen(const Environment &env) const
    -> Outcome<llvm::Type *, Error> {
  auto member_result = member_type->Codegen(env);
  if (!member_result) {
    return member_result;
  }
  auto *member_type = member_result.GetFirst();
  auto *llvm_array_type = llvm::ArrayType::get(member_type, size);

  auto *integer_type = env.types->GetIntType();
  auto integer_type_result = integer_type->Codegen(env);
  if (!integer_type_result) {
    return integer_type_result;
  }
  auto *llvm_integer_type = integer_type_result.GetFirst();

  return {llvm::StructType::get(*env.context,
                                {llvm_integer_type, llvm_array_type})};
}
} // namespace pink
