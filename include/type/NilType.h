/**
 * @file NilType.h
 * @brief Header for class NilType
 * @version 0.1
 */
#pragma once
#include "type/Type.h"

namespace pink {
/**
 * @brief Represents the Type of Nil
 */
class NilType : public Type {
public:
  using Pointer = NilType const *;

  NilType() noexcept : Type(Type::Kind::Nil) {}
  ~NilType() noexcept override                               = default;
  NilType(const NilType &other) noexcept                     = default;
  NilType(NilType &&other) noexcept                          = default;
  auto operator=(const NilType &other) noexcept -> NilType & = default;
  auto operator=(NilType &&other) noexcept -> NilType      & = default;

  static auto classof(const Type* type) noexcept -> bool {
    return Type::Kind::Nil == type->GetKind();
  }

  void Accept(TypeVisitor *visitor) noexcept override { visitor->Visit(this); }
  void Accept(ConstTypeVisitor *visitor) const noexcept override {
    visitor->Visit(this);
  }
};
} // namespace pink
