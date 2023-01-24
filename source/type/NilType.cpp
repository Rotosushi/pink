#include "type/NilType.h"
#include "aux/Environment.h"

namespace pink {
/*
auto NilType::EqualTo(Type *other) const -> bool {
  return other->GetKind() == Type::Kind::Nil;
}

auto NilType::ToString() const -> std::string { return {"Nil"}; }

auto NilType::ToLLVM(const Environment &env) const -> llvm::Type * {
  return env.instruction_builder->getInt1Ty();
}
*/
} // namespace pink
