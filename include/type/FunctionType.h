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
 * @file FunctionType.h
 * @brief Header for class FunctionType
 * @version 0.1
 */
#pragma once
#include <vector>

#include "type/Type.h"

namespace pink {
/**
 * @brief Represents the Type of a Function
 */
class FunctionType : public Type {
public:
  using Arguments      = std::vector<Type::Pointer>;
  using iterator       = Arguments::iterator;
  using const_iterator = Arguments::const_iterator;
  using Pointer        = FunctionType const *;

private:
  Type::Pointer return_type;
  Arguments     arguments;

public:
  FunctionType(TypeInterner *context,
               Type::Pointer return_type,
               Arguments     arguments) noexcept
      : Type(Type::Kind::Function, context),
        return_type(return_type),
        arguments(std::move(arguments)) {
    assert(return_type != nullptr);
  }
  ~FunctionType() noexcept override                = default;
  FunctionType(const FunctionType &other) noexcept = default;
  FunctionType(FunctionType &&other) noexcept      = default;
  auto operator=(const FunctionType &other) noexcept
      -> FunctionType                                           & = default;
  auto operator=(FunctionType &&other) noexcept -> FunctionType & = default;

  [[nodiscard]] auto GetReturnType() noexcept -> Type::Pointer {
    return return_type;
  }
  [[nodiscard]] auto GetReturnType() const noexcept -> Type::Pointer {
    return return_type;
  }
  [[nodiscard]] auto GetArguments() noexcept -> Arguments & {
    return arguments;
  }
  [[nodiscard]] auto GetArguments() const noexcept -> const Arguments & {
    return arguments;
  }

  [[nodiscard]] auto begin() noexcept -> iterator { return arguments.begin(); }
  [[nodiscard]] auto begin() const noexcept -> const_iterator {
    return arguments.begin();
  }
  [[nodiscard]] auto cbegin() const noexcept -> const_iterator {
    return arguments.cbegin();
  }
  [[nodiscard]] auto end() noexcept -> iterator { return arguments.end(); }
  [[nodiscard]] auto end() const noexcept -> const_iterator {
    return arguments.end();
  }
  [[nodiscard]] auto cend() const noexcept -> const_iterator {
    return arguments.cend();
  }

  static auto classof(const Type *type) noexcept -> bool {
    return Type::Kind::Function == type->GetKind();
  }

  void Accept(TypeVisitor *visitor) noexcept override { visitor->Visit(this); }
  void Accept(ConstTypeVisitor *visitor) const noexcept override {
    visitor->Visit(this);
  }
};
} // namespace pink
