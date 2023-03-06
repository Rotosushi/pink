/**
 * @file VoidType.h
 * @brief Header for class VoidType
 * @version 0.1
 */
#pragma once

#include "type/Type.h"

namespace pink {
/**
 * @brief Represents the Type of Void
 */
class VoidType : public Type {
public:
  using Pointer = VoidType const *;

  VoidType(TypeInterner *context) noexcept
      : Type(Type::Kind::Void, context) {}
  ~VoidType() noexcept override                                = default;
  VoidType(const VoidType &other) noexcept                     = default;
  VoidType(VoidType &&other) noexcept                          = default;
  auto operator=(const VoidType &other) noexcept -> VoidType & = default;
  auto operator=(VoidType &&other) noexcept -> VoidType      & = default;

  static auto classof(const Type *type) noexcept -> bool {
    return Type::Kind::Void == type->GetKind();
  }

  void Accept(TypeVisitor *visitor) noexcept override { visitor->Visit(this); }
  void Accept(ConstTypeVisitor *visitor) const noexcept override {
    visitor->Visit(this);
  }
};
} // namespace pink
