#include "type/IntType.h"
#include "aux/Environment.h"

namespace pink {
IntType::IntType() : Type(Type::Kind::Int) {}

auto IntType::classof(const Type *type) -> bool {
  return type->getKind() == Type::Kind::Int;
}

auto IntType::EqualTo(Type *other) const -> bool {
  return other->getKind() == Type::Kind::Int;
}

auto IntType::ToString() const -> std::string { return {"Int"}; }

auto IntType::Codegen(const Environment &env) const
    -> Outcome<llvm::Type *, Error> {
  return {env.instruction_builder->getInt64Ty()};
}
} // namespace pink
