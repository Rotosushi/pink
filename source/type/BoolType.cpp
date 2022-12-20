#include "type/BoolType.h"
#include "aux/Environment.h"

namespace pink {
BooleanType::BooleanType() : Type(Type::Kind::Bool) {}

auto BooleanType::classof(const Type *type) -> bool {
  return type->GetKind() == Type::Kind::Bool;
}

auto BooleanType::EqualTo(Type *other) const -> bool {
  return other->GetKind() == Type::Kind::Bool;
}

auto BooleanType::ToString() const -> std::string { return {"Bool"}; }

auto BooleanType::ToLLVM(const Environment &env) const -> llvm::Type * {
  return env.instruction_builder->getInt1Ty();
}
} // namespace pink
