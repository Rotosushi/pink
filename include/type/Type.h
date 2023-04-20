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
#include <bitset>   // std::bitset
#include <optional> // std::optional
#include <ostream>  // std::ostream
#include <sstream>  // std::stringstream

#include "llvm/IR/Type.h"

namespace pink {
class CompilationUnit;
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
    Variable,
    Integer,
    Nil,
    Pointer,
    Slice,
    Tuple,
    Void,
  };

private:
  Kind                kind;
  /*
   * I dislike using the mutable keyword if
   * at all possible, however this is a member
   * which is only used to avoid recomputing
   * the equivalent llvm::Type if that work
   * has already been done. This does not affect
   * the visible semantics of any particular type.
   */
  mutable llvm::Type *llvm_type;
  /*
   * Context is used by Substitution to simplify
   * it's implementation and call signature.
   * also, since Types are only validly constructable
   * via the TypeInterner, it is less of an issue
   * that they are so tightly coupled.
   */
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

  virtual auto ToLLVM(CompilationUnit &unit) const noexcept -> llvm::Type * = 0;
  virtual auto Equals(Type::Pointer right) const noexcept -> bool           = 0;

  virtual void Print(std::ostream &result) const noexcept = 0;
  auto         ToString() const noexcept -> std::string {
    std::stringstream stream;
    Print(stream);
    return std::move(stream).str();
  }
};

inline auto operator<<(std::ostream &out, const Type &type) -> std::ostream & {
  type.Print(out);
  return out;
}

inline auto operator<<(std::ostream &out, Type::Pointer type)
    -> std::ostream & {
  type->Print(out);
  return out;
}
} // namespace pink

/*
  still debating where this class needs to live.

  basically we are trying to describe the representation
  of the type within the comptime and runtime of the program.

  so, the considerations here are that if we make the annotations
  part of the type itself (a.k.a part of the type signature),
  then we have to answer the question, are the type annotations
  considered when performing structural equality?
  in the simplest sense, obviously yes. a constant T is not equal
  to a mutable T, and we need to be able to differentiate the two.
  ditto for Address backed types and Temporary types.
  however this immediately precludes the typechecker from being able
  to rely upon simple address equality. as the rules for what
  types can bind where are more complex than just, is T == T.
  for instance, when binding a function argument we know that
  we can bind a const T to a const T, and a mutable T to a
  mutable T, however it is also acceptable to bind a mutable T
  to a const T, but it is not acceptable to bind a const T to a
  mutable T. thus we observe that to properly enfore these
  binding rules we must inspect the type more than just (T == T).
  even in the case where the annotations are a part of the type.

  however, in all cases we still want to perform the check (T == T),
  even if the annotations need to be checked further after the fact.
  so we need some way of considering both the type T and it's annotations,
  we need to be able to compare them seperately, and whenever we
  need a type T we also need to know the annotation information.

  one observation I have had is that at least conceptually it makes
  sense to consider that some type T is the same underlying type
  irrespective of it's particular annotations. that is, much like
  how we don't associate a location with the type T, but with a particular
  instance of said type T. we could do the same with the annotations.
  however this is not really possible given the way that we intern
  types themselves. we can observe that any composite type must
  be annotated both on itself, and each of it's component types must
  also carry annotations. (i.e. a pointer to T can itself be const, or
  it's pointee type can be const, or both. meaning that all possiblities
  must be interned to properly support address equality.)

  this might be simpler if we did not enforce address equality.
  it would take more time and space, but it would be conceptually more
  straightforward. as each type is itself a separate instance it itself
  contains the annotations and source location information without
  question.
  to allow for more complex statements it makes sense to be able to separate
  the underlying type from it's annotations as well.
  as in, to answer the question, what is the pointee type of this pointer
  type. or what is the element type of this array type. or what is the nth
  type of this tuple type?

  and in a more general sense to work with the types using
  the syntax and semantics of the language.
  building up types from component parts, and breaking down types into
  their component parts, just as is done with values.

  these two operations are generally going to be implemented via
  Algebraic Data Types, plus primitive types, for composition
  and Match expressions, plus type destructuring, for decomposition

  as a big caveat, none of this is going to be implemented anytime
  soon. we have to get to dynamic memory before we can truly tackle
  ADT, Match, and destructuring.

    class Annotations {
private:
  enum Flags : unsigned {
    InMemory,  // true -> backed by memory (stack, heap),
               // false -> backed by a literal value (llvm::Constant, or
               // similar)
    Constant,  // true -> cannot be written, false -> can be written
    Temporary, // true -> represents a temporary value,
               // false -> represents some longer lifetime
    Comptime,  // true -> known at compile time, false -> known at runtime
               // (for now, link-time known is under runtime)
    SIZE       // must be the last enum value
               // no enums may have an assigned value
  };

  static constexpr auto bitset_size = ToUnderlying(Flags::SIZE);
  using Set                         = std::bitset<bitset_size>;
  Set set;

public:
  [[nodiscard]] auto IsInMemory() const noexcept -> bool {
    return set[InMemory];
  }
  auto IsInMemory(bool state) noexcept -> bool {
    return set[InMemory] = state;
  }

  [[nodiscard]] auto IsConstant() const noexcept -> bool {
    return set[Constant];
  }
  auto IsConstant(bool state) noexcept -> bool {
    return set[Constant] = state;
  }
};
*/
