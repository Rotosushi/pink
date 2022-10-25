#include "type/NilType.h"
#include "aux/Environment.h"

namespace pink {
NilType::NilType() : Type(Type::Kind::Nil) {}

auto NilType::classof(const Type *type) -> bool {
  return type->GetKind() == Type::Kind::Nil;
}

auto NilType::EqualTo(Type *other) const -> bool {
  return other->GetKind() == Type::Kind::Nil;
}

auto NilType::ToString() const -> std::string { return {"Nil"}; }

auto NilType::Codegen(const Environment &env) const -> llvm::Type * {
  return env.instruction_builder->getInt1Ty();
}
} // namespace pink
