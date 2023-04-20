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

#include "llvm/IR/Attributes.h"

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
  Type::Pointer               return_type;
  Arguments                   arguments;
  // I dislike 'mutable' for obvious reasons.
  // the reason it is used is that we only know what attributes
  // to annotate the given function type with once we are
  // processing this function type in ToLLVM.
  // And it makes sense to
  // construct the relevant return and argument attributes
  // when processing the pink::FunctionType into the
  // equivalent llvm::FunctionType. And, we cannot annotate
  // an llvm::FunctionType with Attributes, only an llvm::Function
  // holds Attributes (for our purposes here). So, to avoid
  // transforming FunctionType twice in two separate code locations
  // we want to annotate pink::FunctionType with the relevant
  // Attributes. However, since ToLLVMVisitor is const, we must
  // make this single member mutable. (this is the only modification,
  // of a type that ToLLVM does, and I was resisting adding this)
  // We can think of the AttributeList here as a cache.
  // A) all attributes added to this FunctionType *must* be added
  // to support passing and returning aggregate types for any
  // Function which has this Type.
  // B) any attributes that can only be computed once we are
  // processing the Body of a function may be added with this
  // attribute set as a valid and necessary starting point.
  // C) Any Function that has this Type can validly use these
  // attributes as a starting point for their individualized
  // AttributeList.
  mutable llvm::AttributeList attributes;

public:
  FunctionType(TypeInterner *context,
               Type::Pointer return_type,
               Arguments     arguments) noexcept
      : Type(Type::Kind::Function, context),
        return_type(return_type),
        arguments(std::move(arguments)),
        attributes() {
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
  [[nodiscard]] auto GetAttributes() noexcept -> llvm::AttributeList & {
    return attributes;
  }
  [[nodiscard]] auto GetAttributes() const noexcept
      -> const llvm::AttributeList & {
    return attributes;
  }
  void SetAttributes(const llvm::AttributeList &list) const noexcept {
    attributes = list;
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

  auto ToLLVM(CompilationUnit &unit) const noexcept -> llvm::Type * override;
  auto Equals(Type::Pointer right) const noexcept -> bool override;
  void Print(std::ostream &stream) const noexcept override;
};
} // namespace pink
