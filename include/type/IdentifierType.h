#pragma once

#include "type/Type.h"

#include "aux/StringInterner.h"

namespace pink {
class IdentifierType : public Type {
public:
  using Pointer = IdentifierType const *;

private:
  InternedString identifier;

public:
  static auto classof(Type const *type) noexcept -> bool {
    return type->GetKind() == Type::Kind::Identifier;
  }

  IdentifierType(TypeInterner *context, InternedString identifier) noexcept
      : Type(Type::Kind::Identifier, context),
        identifier(identifier) {}

  [[nodiscard]] auto Identifier() const noexcept -> InternedString {
    return identifier;
  }
};
} // namespace pink
