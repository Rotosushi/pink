#include "type/BoolType.h"
#include "aux/Environment.h"

namespace pink {
BoolType::BoolType() : Type(Type::Kind::Bool) {}

auto BoolType::classof(const Type *type) -> bool {
  return type->GetKind() == Type::Kind::Bool;
}

auto BoolType::EqualTo(Type *other) const -> bool {
  return other->GetKind() == Type::Kind::Bool;
}

auto BoolType::ToString() const -> std::string { return {"Bool"}; }

auto BoolType::Codegen(const Environment &env) const -> llvm::Type * {
  return env.instruction_builder->getInt1Ty();
}
} // namespace pink
