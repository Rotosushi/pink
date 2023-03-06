/**
 * @file BooleanType.h
 * @brief Header for class BooleanType
 * @version 0.1
 */
#pragma once

#include "type/TypeInterface.h"

namespace pink {
/**
 * @brief Represents the Type of Boolean
 */
class BooleanType : public TypeInterface {
public:
  using Pointer = BooleanType const *;

  BooleanType(TypeInterner *context) noexcept
      : TypeInterface(TypeInterface::Kind::Boolean, context) {}
  ~BooleanType() noexcept override                                   = default;
  BooleanType(const BooleanType &other) noexcept                     = default;
  BooleanType(BooleanType &&other) noexcept                          = default;
  auto operator=(const BooleanType &other) noexcept -> BooleanType & = default;
  auto operator=(BooleanType &&other) noexcept -> BooleanType      & = default;

  static auto classof(const TypeInterface *type) noexcept -> bool {
    return TypeInterface::Kind::Boolean == type->GetKind();
  }

  void Accept(TypeVisitor *visitor) noexcept override { visitor->Visit(this); }
  void Accept(ConstTypeVisitor *visitor) const noexcept override {
    visitor->Visit(this);
  }
};
} // namespace pink
