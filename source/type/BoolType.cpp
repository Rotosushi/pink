#include "type/BoolType.h"
#include "aux/Environment.h"

namespace pink {
BooleanType::BooleanType() : Type(Type::Kind::Boolean) {}

auto BooleanType::classof(const Type *type) -> bool {
  return type->GetKind() == Type::Kind::Boolean;
}

auto BooleanType::EqualTo(Type *other) const -> bool {
  return other->GetKind() == Type::Kind::Boolean;
}

auto BooleanType::ToString() const -> std::string { return {"Boolean"}; }

auto BooleanType::ToLLVM(const Environment &env) const -> llvm::Type * {
  return env.instruction_builder->getInt1Ty();
}
} // namespace pink
