/**
 * @file IntegerType.h
 * @brief Header for IntegerType
 * @version 0.1
 */
#pragma once

#include "type/TypeInterface.h"

namespace pink {
/**
 * @brief Represents the Type Integer
 */
class IntegerType : public TypeInterface {
public:
  using Pointer = IntegerType const *;

  IntegerType(TypeInterner *context) noexcept
      : TypeInterface(TypeInterface::Kind::Integer, context) {}
  ~IntegerType() noexcept override                                   = default;
  IntegerType(const IntegerType &other) noexcept                     = default;
  IntegerType(IntegerType &&other) noexcept                          = default;
  auto operator=(const IntegerType &other) noexcept -> IntegerType & = default;
  auto operator=(IntegerType &&other) noexcept -> IntegerType      & = default;

  static auto classof(const TypeInterface *type) noexcept -> bool {
    return TypeInterface::Kind::Integer == type->GetKind();
  }

  void Accept(TypeVisitor *visitor) noexcept override { visitor->Visit(this); }
  void Accept(ConstTypeVisitor *visitor) const noexcept override {
    visitor->Visit(this);
  }
};
} // namespace pink
