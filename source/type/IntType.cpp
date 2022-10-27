#include "type/IntType.h"
#include "aux/Environment.h"

namespace pink {
IntType::IntType() : Type(Type::Kind::Int) {}

auto IntType::classof(const Type *type) -> bool {
  return type->GetKind() == Type::Kind::Int;
}

auto IntType::EqualTo(Type *other) const -> bool {
  return other->GetKind() == Type::Kind::Int;
}

auto IntType::ToString() const -> std::string { return {"Int"}; }

auto IntType::ToLLVM(const Environment &env) const -> llvm::Type * {
  return env.instruction_builder->getInt64Ty();
}
} // namespace pink
