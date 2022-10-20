#include "type/SliceType.h"

#include "aux/Environment.h"

namespace pink {
SliceType::SliceType(Type *pointee_type)
    : Type(Type::Kind::Slice), pointee_type(pointee_type) {}

auto SliceType::classof(const Type *type) -> bool {
  return type->GetKind() == Type::Kind::Slice;
}

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

auto SliceType::Codegen(const Environment &env) const
    -> Outcome<llvm::Type *, Error> {
  llvm::Type *integer_type = env.instruction_builder->getInt64Ty();
  auto *pointer_type = llvm::PointerType::getUnqual(*env.context);
  return {llvm::StructType::get(
      *env.context, {integer_type, integer_type, pointer_type}, false)};
}
} // namespace pink