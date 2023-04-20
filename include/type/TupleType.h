// Copyright (C) 2023 cadence
//
// This file is part of pink.
//
// pink is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// pink is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with pink.  If not, see <http://www.gnu.org/licenses/>.

/**
 * @file TupleType.h
 * @brief Header for class TupleType
 * @version 0.1
 */
#pragma once
#include <vector>

#include "type/Type.h"

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

  auto ToLLVM(CompilationUnit &unit) const noexcept -> llvm::Type * override;
  auto Equals(Type::Pointer right) const noexcept -> bool override;
  void Print(std::ostream &stream) const noexcept override;
};
} // namespace pink
