/**
 * @file IntegerType.h
 * @brief Header for IntegerType
 * @version 0.1
 */
#pragma once

#include "type/Type.h"

namespace pink {
/**
 * @brief Represents the Type Integer
 */
class IntegerType : public Type {
public:
  using Pointer = IntegerType const *;

  IntegerType(TypeInterner *context) noexcept
      : Type(Type::Kind::Integer, context) {}
  ~IntegerType() noexcept override                                   = default;
  IntegerType(const IntegerType &other) noexcept                     = default;
  IntegerType(IntegerType &&other) noexcept                          = default;
  auto operator=(const IntegerType &other) noexcept -> IntegerType & = default;
  auto operator=(IntegerType &&other) noexcept -> IntegerType      & = default;

  static auto classof(const Type *type) noexcept -> bool {
    return Type::Kind::Integer == type->GetKind();
  }

  void Accept(TypeVisitor *visitor) noexcept override { visitor->Visit(this); }
  void Accept(ConstTypeVisitor *visitor) const noexcept override {
    visitor->Visit(this);
  }
};
} // namespace pink
