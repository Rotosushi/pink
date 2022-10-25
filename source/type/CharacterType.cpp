#include "type/CharacterType.h"

#include "aux/Environment.h"

namespace pink {
CharacterType::CharacterType() : Type(Type::Kind::Character) {}

auto CharacterType::classof(const Type *type) -> bool {
  return type->GetKind() == Type::Kind::Character;
}

auto CharacterType::EqualTo(Type *other) const -> bool {
  return (llvm::dyn_cast<CharacterType>(other) != nullptr);
}

auto CharacterType::ToString() const -> std::string { return "Character"; }

auto CharacterType::Codegen(const Environment &env) const -> llvm::Type * {
  return env.instruction_builder->getInt8Ty();
}

} // namespace pink