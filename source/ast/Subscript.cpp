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
#include "ast/Subscript.h"

#include "aux/Environment.h"

namespace pink {
/*
The type of a subscript operation is the element_type of the
subscriptable pointer being accessed if and only if the left
expression has slice or array type, and the right expression
has Integer type.

the lhs of a subscript can be a literal or in memory, it can
be constant or mutable, and it can be temporary or not.
the rhs of a subscript can be a literal or in memory, it can
be constant or mutable, and it can be temporary or not.

the result type is dependent upon the element type of the array.
*/
auto Subscript::Typecheck(CompilationUnit &unit) const noexcept
    -> Outcome<Type::Pointer> {
  auto right_outcome = right->Typecheck(unit);
  if (!right_outcome) {
    return right_outcome;
  }
  auto right_type = right_outcome.GetFirst();

  // #TODO iff the index is known at compile type do the range check
  // I think the strategy here can be,
  // 1) check if the type is an Integer,
  //  if no -> fail here (as we do now)
  // 2) check if the integer type is a literal type
  //    if type is in memory -> move on to the rest of the
  //      typechecking routine
  //    if the type is literal -> evaluate the rhs to it's literal value
  //    do the range check on the value
  // we can further generalize this strategy for each location where
  // we could make use of some value at compile time.
  // Though i am somewhat concerned about the validity of interleaving
  // typechecking and interpretation. This might be tricky to get right.
  Type::Annotations annotations;
  annotations.IsInMemory(false);
  if (!Equals(right_type, unit.GetIntType(annotations))) {
    std::stringstream errmsg;
    errmsg << "Cannot use type [";
    errmsg << right_type;
    errmsg << "] as an index.";
    return Error(Error::Code::SubscriptRightIsNotAnIndex,
                 right->GetLocation(),
                 std::move(errmsg).str());
  }

  auto left_outcome = left->Typecheck(unit);
  if (!left_outcome) {
    return left_outcome;
  }
  auto left_type = left_outcome.GetFirst();

  auto element_outcome = [&]() -> Outcome<Type::Pointer> {
    if (const auto *array_type = llvm::dyn_cast<ArrayType>(left_type);
        array_type != nullptr) {
      return array_type->GetElementType();
    }

    if (const auto *slice_type = llvm::dyn_cast<SliceType>(left_type);
        slice_type != nullptr) {
      return slice_type->GetPointeeType();
    }

    std::stringstream errmsg;
    errmsg << "Cannot subscript type [";
    errmsg << left_type;
    errmsg << "]";
    return Error(Error::Code::SubscriptLeftIsNotSubscriptable,
                 left->GetLocation(),
                 std::move(errmsg).str());
  }();
  if (!element_outcome) {
    return element_outcome;
  }
  const auto *element_type = element_outcome.GetFirst();
  SetCachedType(element_type);
  return element_type;
}

auto Subscript::Codegen(CompilationUnit &unit) const noexcept
    -> Outcome<llvm::Value *> {
  auto left_outcome = left->Codegen(unit);
  if (!left_outcome) {
    return left_outcome;
  }
  auto *left_value = left_outcome.GetFirst();
  assert(left_value != nullptr);
  auto left_type = left->GetCachedTypeOrAssert();

  auto right_outcome = [&]() {
    // #NOTE: a subscript term on the LHS of assignment
    // must not suppress any loads of variables occuring
    // within the evaluation of it's index expression.
    // so we pretend like we are not on the LHS of assignment.
    if (unit.OnTheLHSOfAssignment()) {
      unit.OnTheLHSOfAssignment(false);
      auto right_outcome = right->Codegen(unit);
      unit.OnTheLHSOfAssignment(true);
      return right_outcome;
    }
    return right->Codegen(unit);
  }();
  if (!right_outcome) {
    return right_outcome;
  }
  auto right_value = right_outcome.GetFirst();

  if (const auto *array_type = llvm::dyn_cast<ArrayType const>(left_type);
      array_type != nullptr) {
    auto *llvm_array_type =
        llvm::cast<llvm::StructType>(array_type->ToLLVM(unit));
    return unit.ArraySubscript(llvm_array_type, left_value, right_value);
  }

  if (const auto *slice_type = llvm::dyn_cast<SliceType const>(left_type);
      slice_type != nullptr) {
    auto *llvm_slice_type =
        llvm::cast<llvm::StructType>(slice_type->ToLLVM(unit));
    auto *llvm_element_type = slice_type->GetPointeeType()->ToLLVM(unit);
    return unit.SliceSubscript(llvm_slice_type,
                               llvm_element_type,
                               left_value,
                               right_value);
  }

  // we should never reach here, just in case
  assert(false);
}

void Subscript::Print(std::ostream &stream) const noexcept {
  stream << left << "[" << right << "]";
}
} // namespace pink
