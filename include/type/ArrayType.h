/**
 * @file ArrayType.h
 * @brief Header for class ArrayType
 * @version 0.1
 */
#pragma once
#include "type/Type.h"

namespace pink {
/**
 * @brief Represents the Type of an Array
 */
class ArrayType : public Type {
public:
  using Pointer = ArrayType const *;

private:
  std::size_t   size;
  Type::Pointer element_type;

public:
  ArrayType(std::size_t size, Type::Pointer element_type) noexcept
      : Type(Type::Kind::Array), size(size), element_type(element_type) {
    assert(element_type != nullptr);
  }
  ~ArrayType() noexcept override                                 = default;
  ArrayType(const ArrayType &other) noexcept                     = default;
  ArrayType(ArrayType &&other) noexcept                          = default;
  auto operator=(const ArrayType &other) noexcept -> ArrayType & = default;
  auto operator=(ArrayType &&other) noexcept -> ArrayType      & = default;

  [[nodiscard]] auto GetSize() const -> std::size_t { return size; }
  [[nodiscard]] auto GetElementType() const -> Type::Pointer {
    return element_type;
  }

  static auto classof(const Type::Pointer type) noexcept -> bool {
    return Type::Kind::Array == type->GetKind();
  }

  void Accept(TypeVisitor *visitor) noexcept override { visitor->Visit(this); }
  void Accept(ConstTypeVisitor *visitor) const noexcept override {
    visitor->Visit(this);
  }
};
} // namespace pink
