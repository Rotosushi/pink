#include "type/IntType.h"
#include "aux/Environment.h"

namespace pink {
IntegerType::IntegerType() : Type(Type::Kind::Integer) {}

auto IntegerType::classof(const Type *type) -> bool {
  return type->GetKind() == Type::Kind::Integer;
}

auto IntegerType::EqualTo(Type *other) const -> bool {
  return other->GetKind() == Type::Kind::Integer;
}

auto IntegerType::ToString() const -> std::string { return {"Integer"}; }

auto IntegerType::ToLLVM(const Environment &env) const -> llvm::Type * {
  return env.instruction_builder->getInt64Ty();
}
} // namespace pink
