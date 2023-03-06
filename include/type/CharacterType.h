#pragma once
#include "type/TypeInterface.h"

namespace pink {
class CharacterType : public TypeInterface {
public:
  using Pointer = CharacterType const *;

  CharacterType(TypeInterner *context) noexcept
      : TypeInterface(TypeInterface::Kind::Character, context) {}
  ~CharacterType() noexcept override                 = default;
  CharacterType(const CharacterType &other) noexcept = default;
  CharacterType(CharacterType &&other) noexcept      = default;
  auto operator=(const CharacterType &other) noexcept
      -> CharacterType                                            & = default;
  auto operator=(CharacterType &&other) noexcept -> CharacterType & = default;

  static auto classof(const TypeInterface *type) noexcept -> bool {
    return TypeInterface::Kind::Character == type->GetKind();
  }

  void Accept(TypeVisitor *visitor) noexcept override { visitor->Visit(this); }
  void Accept(ConstTypeVisitor *visitor) const noexcept override {
    visitor->Visit(this);
  }
};
} // namespace pink