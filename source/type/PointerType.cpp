#include "type/PointerType.h"
#include "aux/Environment.h"

#include "llvm/IR/DerivedTypes.h"

namespace pink {
PointerType::PointerType(Type *pointee_type)
    : Type(Type::Kind::Pointer), pointee_type(pointee_type) {}

auto PointerType::classof(const Type *type) -> bool {
  return type->GetKind() == Type::Kind::Pointer;
}

auto PointerType::EqualTo(Type *other) const -> bool {
  bool equal = true;
  auto *pointer_type = llvm::dyn_cast<PointerType>(other);
  if (pointer_type != nullptr) {
    if (pointer_type->pointee_type != this->pointee_type) {
      equal = false;
    }
  } else {
    equal = false;
  }

  return equal;
}

auto PointerType::ToString() const -> std::string {
  std::string result;

  result += "ptr ";
  result += pointee_type->ToString();

  return result;
}

auto PointerType::Codegen(const Environment &env) const -> llvm::Type * {
  // \note llvm uses opaque pointers
  return env.instruction_builder->getPtrTy();
}

} // namespace pink
