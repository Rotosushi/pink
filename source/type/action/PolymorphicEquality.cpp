// Copyright (C) 2023 Cade Weinberg
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

#include "type/action/PolymorphicEquality.h"
#include "type/action/ToString.h"

#include "type/visitor/TypeVisitor.h"
#include "visitor/VisitorResult.h"

#include "type/All.h"

namespace pink {
class PolymorphicEqualityVisitor
    : public ConstVisitorResult<PolymorphicEqualityVisitor,
                                const Type::Pointer,
                                PolymorphicEqualityResult>,
      public ConstTypeVisitor {
private:
  Type::Pointer left;

public:
  void Visit(const ArrayType *array_type) const noexcept override;
  void Visit(const BooleanType *boolean_type) const noexcept override;
  void Visit(const CharacterType *character_type) const noexcept override;
  void Visit(const FunctionType *function_type) const noexcept override;
  void Visit(const TypeVariable *type_variable) const noexcept override;
  void Visit(const IntegerType *integer_type) const noexcept override;
  void Visit(const NilType *nil_type) const noexcept override;
  void Visit(const PointerType *pointer_type) const noexcept override;
  void Visit(const SliceType *slice_type) const noexcept override;
  void Visit(const TupleType *tuple_type) const noexcept override;
  void Visit(const VoidType *void_type) const noexcept override;

  PolymorphicEqualityVisitor(Type::Pointer left) noexcept
      : ConstVisitorResult{},
        left{left} {}
};

void PolymorphicEqualityVisitor::Visit(
    const ArrayType *array_type) const noexcept {
  if (const auto *array_left = llvm::dyn_cast<ArrayType>(left);
      array_left != nullptr) {
    if (array_left->GetSize() != array_type->GetSize()) {
      std::string errmsg = "left size [";
      errmsg             += std::to_string(array_left->GetSize());
      errmsg             += "], right size [";
      errmsg             += std::to_string(array_type->GetSize());
      errmsg             += "]";
      result             = Error{Error::Code::ArraySizeMismatch, {}, errmsg};
      return;
    }

    result = PolymorphicEquality(array_type->GetElementType(),
                                 array_left->GetElementType());
    return;
  }

  std::string errmsg = "Cannot substitute type [";
  errmsg             += ToString(left);
  errmsg             += "] for type [";
  errmsg             += ToString(array_type);
  errmsg             += "]";
  result             = Error{Error::Code::TypeSubstitutionInvalid, {}, errmsg};
}

void PolymorphicEqualityVisitor::Visit(
    const BooleanType *boolean_type) const noexcept {
  if (const auto *left_boolean = llvm::dyn_cast<BooleanType>(left);
      left_boolean != nullptr) {
    result = PolymorphicEqualityResult{};
  }

  std::string errmsg = "Cannot substitute type [";
  errmsg             += ToString(left);
  errmsg             += "] for type [";
  errmsg             += ToString(boolean_type);
  errmsg             += "]";
  result             = Error{Error::Code::TypeSubstitutionInvalid, {}, errmsg};
}

void PolymorphicEqualityVisitor::Visit(
    const CharacterType *character_type) const noexcept {
  if (const auto *left_character = llvm::dyn_cast<CharacterType>(left);
      left_character != nullptr) {
    result = PolymorphicEqualityResult{};
  }

  std::string errmsg = "Cannot substitute type [";
  errmsg             += ToString(left);
  errmsg             += "] for type [";
  errmsg             += ToString(character_type);
  errmsg             += "]";
  result             = Error{Error::Code::TypeSubstitutionInvalid, {}, errmsg};
}

void PolymorphicEqualityVisitor::Visit(
    const FunctionType *function_type) const noexcept {
  if (const auto *left_function = llvm::dyn_cast<FunctionType>(left);
      left_function != nullptr) {
    auto equality_error = PolymorphicEquality(left_function->GetReturnType(),
                                              function_type->GetReturnType());
    if (equality_error) {
      result = equality_error;
      return;
    }

    auto left_iterator  = left_function->begin();
    auto left_end       = left_function->end();
    auto right_iterator = function_type->begin();
    while (left_iterator != left_end) {
      const auto *left_argument_type  = *left_iterator;
      const auto *right_argument_type = *right_iterator;

      auto equality_error =
          PolymorphicEquality(left_argument_type, right_argument_type);
      if (equality_error) {
        result = equality_error;
        return;
      }

      left_iterator++;
      right_iterator++;
    }

    result = PolymorphicEqualityResult{};
    return;
  }

  std::string errmsg = "Cannot substitute type [";
  errmsg             += ToString(left);
  errmsg             += "] for type [";
  errmsg             += ToString(function_type);
  errmsg             += "]";
  result             = Error{Error::Code::TypeSubstitutionInvalid, {}, errmsg};
}

/*
  A type variable is polymorphically equal to any monomorphic type.

  that is, it is valid to substitute the type variable with
  any monomorphic type.
*/
void PolymorphicEqualityVisitor::Visit(
    [[maybe_unused]] const TypeVariable *type_variable) const noexcept {
  if (const auto *poly_left = llvm::dyn_cast<TypeVariable>(left);
      poly_left != nullptr) {
    std::string errmsg = "Cannot substitute type variable [";
    errmsg             += ToString(poly_left);
    errmsg             += "] for type variable [";
    errmsg             += ToString(type_variable);
    errmsg             += "]";
    result = Error{Error::Code::TypeSubstitutionInvalid, {}, errmsg};
    return;
  }
  result = PolymorphicEqualityResult{};
}

void PolymorphicEqualityVisitor::Visit(
    const IntegerType *integer_type) const noexcept {
  if (const auto *left_integer = llvm::dyn_cast<IntegerType>(left);
      left_integer != nullptr) {
    result = PolymorphicEqualityResult{};
  }

  std::string errmsg = "Cannot substitute type [";
  errmsg             += ToString(left);
  errmsg             += "] for type [";
  errmsg             += ToString(integer_type);
  errmsg             += "]";
  result             = Error{Error::Code::TypeSubstitutionInvalid, {}, errmsg};
}

void PolymorphicEqualityVisitor::Visit(const NilType *nil_type) const noexcept {
  if (const auto *left_nil = llvm::dyn_cast<NilType>(left);
      left_nil != nullptr) {
    result = PolymorphicEqualityResult{};
  }

  std::string errmsg = "Cannot substitute type [";
  errmsg             += ToString(left);
  errmsg             += "] for type [";
  errmsg             += ToString(nil_type);
  errmsg             += "]";
  result             = Error{Error::Code::TypeSubstitutionInvalid, {}, errmsg};
}

void PolymorphicEqualityVisitor::Visit(
    const PointerType *pointer_type) const noexcept {
  if (const auto *left_pointer = llvm::dyn_cast<PointerType>(left);
      left_pointer != nullptr) {
    result = PolymorphicEquality(left_pointer->GetPointeeType(),
                                 pointer_type->GetPointeeType());
  }

  std::string errmsg = "Cannot substitute type [";
  errmsg             += ToString(left);
  errmsg             += "] for type [";
  errmsg             += ToString(pointer_type);
  errmsg             += "]";
  result             = Error{Error::Code::TypeSubstitutionInvalid, {}, errmsg};
}

void PolymorphicEqualityVisitor::Visit(
    const SliceType *slice_type) const noexcept {
  if (const auto *left_pointer = llvm::dyn_cast<SliceType>(left);
      left_pointer != nullptr) {
    result = PolymorphicEquality(left_pointer->GetPointeeType(),
                                 slice_type->GetPointeeType());
  }

  std::string errmsg = "Cannot substitute type [";
  errmsg             += ToString(left);
  errmsg             += "] for type [";
  errmsg             += ToString(slice_type);
  errmsg             += "]";
  result             = Error{Error::Code::TypeSubstitutionInvalid, {}, errmsg};
}

void PolymorphicEqualityVisitor::Visit(
    const TupleType *tuple_type) const noexcept {
  if (const auto *left_tuple = llvm::dyn_cast<TupleType>(left);
      left_tuple != nullptr) {
    if (tuple_type->GetElements().size() != left_tuple->GetElements().size()) {
      std::string errmsg = "left tuple [";
      errmsg             += ToString(left_tuple);
      errmsg             += "], right tuple [";
      errmsg             += ToString(tuple_type);
      errmsg             += "]";
      result             = Error{Error::Code::TupleSizeMismatch, {}, errmsg};
      return;
    }

    auto left_iterator  = left_tuple->begin();
    auto left_end       = left_tuple->end();
    auto right_iterator = tuple_type->begin();
    while (left_iterator != left_end) {
      auto left_element  = *left_iterator;
      auto right_element = *right_iterator;

      if (!PolymorphicEquality(left_element, right_element)) {
        std::string errmsg = "left element [";
        errmsg             += ToString(left_element);
        errmsg             += "], right element [";
        errmsg             += ToString(right_element);
        errmsg             += "]";
        result = Error{Error::Code::TupleElementMismatch, {}, errmsg};
        return;
      }

      left_iterator++;
      right_iterator++;
    }

    result = PolymorphicEqualityResult{};
    return;
  }

  std::string errmsg = "Cannot substitute type [";
  errmsg             += ToString(left);
  errmsg             += "] for type [";
  errmsg             += ToString(tuple_type);
  errmsg             += "]";
  result             = Error{Error::Code::TypeSubstitutionInvalid, {}, errmsg};
}

void PolymorphicEqualityVisitor::Visit(
    const VoidType *void_type) const noexcept {
  if (const auto *left_void = llvm::dyn_cast<VoidType>(left);
      left_void != nullptr) {
    result = PolymorphicEqualityResult{};
  }

  std::string errmsg = "Cannot substitute type [";
  errmsg             += ToString(left);
  errmsg             += "] for type [";
  errmsg             += ToString(void_type);
  errmsg             += "]";
  result             = Error{Error::Code::TypeSubstitutionInvalid, {}, errmsg};
}

/*
  Polymorphic equality is very similar to structural equality, except that
  when we compare a TypeVariable against any other type, we return 'true'

  otherwise, all other structure of the given types must be equal.

  this means Pointer<T> is poly-equal to Pointer<Integer> and Pointer<Boolean>
  or any other Pointer<SomeType>

  this might be weird, but (T, Boolean) is poly-equal to (Integer, Boolean)
  and (Boolean, Boolean), but not (Boolean, Integer).

  [T; 5] is poly-equal to any array of length 5,

  I can't think of use cases for a polymorphic argument type like
  (T, Boolean), but it isn't technically unsound. It's just like,
  what does the boolean even really mean in this context? idk, but
  since it falls out of this definition, it's being kept for now.
  (or until I think of a different implmenetation of templates.)
*/
[[nodiscard]] auto PolymorphicEquality(Type::Pointer left,
                                       Type::Pointer right) noexcept
    -> PolymorphicEqualityResult {
  PolymorphicEqualityVisitor visitor{left};
  return visitor.Compute(right, &visitor);
}

} // namespace pink
