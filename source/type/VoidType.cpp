#include "type/VoidType.h"

#include "aux/Environment.h"

namespace pink {
VoidType::VoidType() : Type(Type::Kind::Void) {}

auto VoidType::classof(const Type *type) -> bool {
  return type->GetKind() == Type::Kind::Void;
}

auto VoidType::EqualTo(Type *other) const -> bool {
  return other->GetKind() == Type::Kind::Void;
}

auto VoidType::ToString() const -> std::string { return {"Void"}; }

auto VoidType::ToLLVM(const Environment &env) const -> llvm::Type * {
  return env.instruction_builder->getVoidTy();
}
} // namespace pink
