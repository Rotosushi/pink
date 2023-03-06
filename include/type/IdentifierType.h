#pragma once

#include "type/TypeInterface.h"

#include "aux/StringInterner.h"

namespace pink {
class TypeVariable : public TypeInterface {
public:
  using Pointer = TypeVariable const *;

private:
  InternedString identifier;

public:
  TypeVariable(TypeInterner *context, InternedString identifier) noexcept
      : TypeInterface(TypeInterface::Kind::Identifier, context),
        identifier(identifier) {}

  static auto classof(TypeInterface const *type) noexcept -> bool {
    return type->GetKind() == TypeInterface::Kind::Identifier;
  }

  [[nodiscard]] auto Identifier() const noexcept -> InternedString {
    return identifier;
  }

  void Accept(TypeVisitor *visitor) noexcept override { visitor->Visit(this); }
  void Accept(ConstTypeVisitor *visitor) const noexcept override {
    visitor->Visit(this);
  }
};
} // namespace pink
