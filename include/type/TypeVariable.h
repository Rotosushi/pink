#pragma once

#include "type/Type.h"

#include "aux/StringInterner.h"

namespace pink {
class TypeVariable : public Type {
public:
  using Pointer = TypeVariable const *;

private:
  InternedString identifier;

public:
  TypeVariable(TypeInterner *context, InternedString identifier) noexcept
      : Type(Type::Kind::Identifier, context),
        identifier(identifier) {}

  static auto classof(Type const *type) noexcept -> bool {
    return type->GetKind() == Type::Kind::Identifier;
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
