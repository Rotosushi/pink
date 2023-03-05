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
  IdentifierType(InternedString identifier) noexcept
      : Type(Type::Kind::Identifier),
        identifier(identifier) {}

  [[nodiscard]] auto Identifier() const noexcept -> InternedString {
    return identifier;
  }
};
} // namespace pink
