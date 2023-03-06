/**
 * @file VoidType.h
 * @brief Header for class VoidType
 * @version 0.1
 */
#pragma once

#include "type/TypeInterface.h"

namespace pink {
/**
 * @brief Represents the Type of Void
 */
class VoidType : public TypeInterface {
public:
  using Pointer = VoidType const *;

  VoidType(TypeInterner *context) noexcept
      : TypeInterface(TypeInterface::Kind::Void, context) {}
  ~VoidType() noexcept override                                = default;
  VoidType(const VoidType &other) noexcept                     = default;
  VoidType(VoidType &&other) noexcept                          = default;
  auto operator=(const VoidType &other) noexcept -> VoidType & = default;
  auto operator=(VoidType &&other) noexcept -> VoidType      & = default;

  static auto classof(const TypeInterface *type) noexcept -> bool {
    return TypeInterface::Kind::Void == type->GetKind();
  }

  void Accept(TypeVisitor *visitor) noexcept override { visitor->Visit(this); }
  void Accept(ConstTypeVisitor *visitor) const noexcept override {
    visitor->Visit(this);
  }
};
} // namespace pink
