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
 * @file Type.h
 * @brief Header for class Type
 * @version 0.1
 */
#pragma once
#include <optional>

#include "llvm/IR/Type.h"

#include "type/visitor/TypeVisitor.h"

namespace pink {
class Environment;
class TypeInterner;

/**
 * @brief Represents an instance of a Type
 *
 * \note Type is pure virtual
 *
 */
class Type {
public:
  using Pointer = Type const *;

  /**
   * @brief Type::Kind is defined so as to conform to LLVM style [RTTI]
   *
   * [RTTI]: https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html "RTTI"
   *
   */
  enum class Kind {
    Array,
    Boolean,
    Character,
    Function,
    Identifier,
    Integer,
    Nil,
    Pointer,
    Slice,
    Tuple,
    Void,
  };

private:
  Kind                kind;
  mutable llvm::Type *llvm_type;
  TypeInterner       *context;

public:
  Type(Kind kind, TypeInterner *context) noexcept
      : kind{kind},
        llvm_type{nullptr},
        context{context} {
    assert(context != nullptr);
  }
  virtual ~Type() noexcept                             = default;
  Type(const Type &other) noexcept                     = default;
  Type(Type &&other) noexcept                          = default;
  auto operator=(const Type &other) noexcept -> Type & = default;
  auto operator=(Type &&other) noexcept -> Type      & = default;

  [[nodiscard]] auto GetKind() const -> Kind { return kind; }
  [[nodiscard]] auto GetContext() const -> TypeInterner * { return context; }

  void SetCachedLLVMType(llvm::Type *llvm_type) const noexcept {
    this->llvm_type = llvm_type;
  }

  auto CachedLLVMType() const noexcept -> std::optional<llvm::Type *> {
    if (llvm_type == nullptr) {
      return {};
    }
    return {llvm_type};
  }

  auto CachedLLVMTypeOrAssert() const noexcept -> llvm::Type * {
    assert(llvm_type != nullptr);
    return llvm_type;
  }

  virtual void Accept(TypeVisitor *vistor) noexcept             = 0;
  virtual void Accept(ConstTypeVisitor *visitor) const noexcept = 0;
};
} // namespace pink
