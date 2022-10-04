#include "type/NilType.h"
#include "aux/Environment.h"

namespace pink {
NilType::NilType() : Type(Type::Kind::Nil) {}

auto NilType::classof(const Type *type) -> bool {
  return type->getKind() == Type::Kind::Nil;
}

auto NilType::EqualTo(Type *other) const -> bool {
  return other->getKind() == Type::Kind::Nil;
}

auto NilType::ToString() const -> std::string { return {"Nil"}; }

auto NilType::Codegen(const Environment &env) const
    -> Outcome<llvm::Type *, Error> {
  return {env.instruction_builder->getInt1Ty()};
}
} // namespace pink
