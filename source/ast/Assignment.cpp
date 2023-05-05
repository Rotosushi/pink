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
#include "ast/Assignment.h"

#include "aux/Environment.h"

namespace pink {
/* 1/24/2023
  The type of an assignment is the Type of it's left hand side,
  if and only if the right hand side is the same type

  the lhs of an assignment must be a mutable, in memory type
  (I think this implies runtime and non-temporary).
  the rhs can be comptime or runtime, literal or in memory, const or mutable,
  temporary or not.
*/
auto Assignment::Typecheck(CompilationUnit &unit) const noexcept
    -> Outcome<Type::Pointer> {
  unit.OnTheLHSOfAssignment(true);
  auto left_outcome = left->Typecheck(unit);
  if (!left_outcome) {
    unit.OnTheLHSOfAssignment(false);
    return left_outcome;
  }
  unit.OnTheLHSOfAssignment(false);
  auto left_type = left_outcome.GetFirst();

  auto right_outcome = right->Typecheck(unit);
  if (!right_outcome) {
    return right_outcome;
  }
  auto right_type = right_outcome.GetFirst();

  if (!Equals(left_type, right_type)) {
    std::stringstream errmsg;
    errmsg << "left type [";
    errmsg << left_type;
    errmsg << "] does not match right type [";
    errmsg << right_type;
    errmsg << "]";
    return Error(Error::Code::AssigneeTypeMismatch,
                 GetLocation(),
                 std::move(errmsg).str());
  }

  SetCachedType(left_type);
  return left_type;
}

/*
  a = b

  a is assigned the value of b.
  or a copy of b is made at the address of a.

*/
auto Assignment::Codegen(CompilationUnit &unit) const noexcept
    -> Outcome<llvm::Value *> {
  unit.OnTheLHSOfAssignment(true);
  auto left_outcome = left->Codegen(unit);
  if (!left_outcome) {
    unit.OnTheLHSOfAssignment(false);
    return left_outcome;
  }
  unit.OnTheLHSOfAssignment(false);
  auto left_value = left_outcome.GetFirst();

  auto right_outcome = right->Codegen(unit);
  if (!right_outcome) {
    return right_outcome;
  }
  auto right_value = right_outcome.GetFirst();

  auto right_type  = right->GetCachedTypeOrAssert();
  auto stored_type = ToLLVM(right_type, unit);

  unit.Store(stored_type, right_value, left_value);
  return right_value;
}

void Assignment::Print(std::ostream &stream) const noexcept {
  stream << left << " = " << right;
}
} // namespace pink
