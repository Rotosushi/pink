/**
 * @file TupleType.h
 * @brief Header for class TupleType
 * @version 0.1
 */
#pragma once
#include <vector>

#include "type/TypeInterface.h"

namespace pink {
/**
 * @brief Represents the Type of a Tuple
 */
class TupleType : public Type {
public:
  using Elements       = std::vector<Type::Pointer>;
  using iterator       = Elements::iterator;
  using const_iterator = Elements::const_iterator;
  using Pointer        = TupleType const *;

private:
  Elements elements;

public:
  TupleType(TypeInterner *context, Elements elements) noexcept
      : Type(Type::Kind::Tuple, context),
        elements(std::move(elements)) {}
  ~TupleType() noexcept override                                 = default;
  TupleType(const TupleType &other) noexcept                     = default;
  TupleType(TupleType &&other) noexcept                          = default;
  auto operator=(const TupleType &other) noexcept -> TupleType & = default;
  auto operator=(TupleType &&other) noexcept -> TupleType      & = default;

  [[nodiscard]] auto GetElements() noexcept -> Elements & { return elements; }
  [[nodiscard]] auto GetElements() const noexcept -> const Elements & {
    return elements;
  }

  [[nodiscard]] auto begin() noexcept -> iterator { return elements.begin(); }
  [[nodiscard]] auto begin() const noexcept -> const_iterator {
    return elements.begin();
  }
  [[nodiscard]] auto cbegin() const noexcept -> const_iterator {
    return elements.cbegin();
  }
  [[nodiscard]] auto end() noexcept -> iterator { return elements.end(); }
  [[nodiscard]] auto end() const noexcept -> const_iterator {
    return elements.end();
  }
  [[nodiscard]] auto cend() const noexcept -> const_iterator {
    return elements.cend();
  }

  static auto classof(const Type *type) noexcept -> bool {
    return Type::Kind::Tuple == type->GetKind();
  }

  void Accept(TypeVisitor *visitor) noexcept override { visitor->Visit(this); }
  void Accept(ConstTypeVisitor *visitor) const noexcept override {
    visitor->Visit(this);
  }
};
} // namespace pink
