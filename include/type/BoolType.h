/**
 * @file BooleanType.h
 * @brief Header for class BooleanType
 * @version 0.1
 */
#pragma once

#include "type/Type.h"

namespace pink {
/**
 * @brief Represents the Type of Boolean
 */
class BooleanType : public Type {
public:
  BooleanType() noexcept : Type(Type::Kind::Boolean) {}
  ~BooleanType() noexcept override = default;
  BooleanType(const BooleanType &other) noexcept = default;
  BooleanType(BooleanType &&other) noexcept = default;
  auto operator=(const BooleanType &other) noexcept -> BooleanType & = default;
  auto operator=(BooleanType &&other) noexcept -> BooleanType & = default;

  static auto classof(const Type *type) noexcept -> bool {
    return Type::Kind::Boolean == type->GetKind();
  }

  void Accept(TypeVisitor *visitor) noexcept override { visitor->Visit(this); }
  void Accept(ConstTypeVisitor *visitor) const noexcept override {
    visitor->Visit(this);
  }
};
} // namespace pink
