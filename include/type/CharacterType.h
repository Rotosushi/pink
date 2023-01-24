#pragma once
#include "type/Type.h"

namespace pink {
class CharacterType : public Type {
public:
  CharacterType() noexcept : Type(Type::Kind::Character) {}
  ~CharacterType() noexcept override = default;
  CharacterType(const CharacterType &other) noexcept = default;
  CharacterType(CharacterType &&other) noexcept = default;
  auto operator=(const CharacterType &other) noexcept
      -> CharacterType & = default;
  auto operator=(CharacterType &&other) noexcept -> CharacterType & = default;

  static auto classof(const Type *type) noexcept -> bool {
    return Type::Kind::Character == type->GetKind();
  }

  void Accept(TypeVisitor *visitor) noexcept override { visitor->Visit(this); }
  void Accept(ConstTypeVisitor *visitor) const noexcept override {
    visitor->Visit(this);
  }
};
} // namespace pink