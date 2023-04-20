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
#include "ast/Conditional.h"

#include "aux/Environment.h"

namespace pink {
/* 1/24/2023
The type of a IfThenElse expression is the type of the first of it's
alternative expressions if and only if the test expression has type
Boolean, and both alternative expressions have the same type.

The test expression can work using a boolean type that is const or mutable,
in memory or a literal, temporary or non-temporary, comptime or runtime.

the then and else blocks are blocks, and so they can be in memory or literal,
const or mutable, temporary or non-temporary.
*/
auto IfThenElse::Typecheck(CompilationUnit &unit) const noexcept
    -> Outcome<Type::Pointer, Error> {
  auto test_outcome = test->Typecheck(unit);
  if (!test_outcome) {
    return test_outcome;
  }
  auto test_type = test_outcome.GetFirst();

  if (!Equals(test_type, unit.GetBoolType())) {
    std::stringstream errmsg;
    errmsg << "Test expression has type [";
    errmsg << test_type;
    errmsg << "] expected type [Boolean]";
    return Error(Error::Code::CondTestExprTypeMismatch,
                 test->GetLocation(),
                 std::move(errmsg).str());
  }

  auto first_outcome = first->Typecheck(unit);
  if (!first_outcome) {
    return first_outcome;
  }
  auto first_type = first_outcome.GetFirst();

  auto second_outcome = second->Typecheck(unit);
  if (!second_outcome) {
    return second_outcome;
  }
  auto second_type = second_outcome.GetFirst();

  if (!Equals(first_type, second_type)) {
    std::stringstream errmsg;
    errmsg << "first type [";
    errmsg << first_type;
    errmsg << "] second type [";
    errmsg << second_type;
    errmsg << "]";
    return Error(Error::Code::CondBodyExprTypeMismatch,
                 GetLocation(),
                 std::move(errmsg).str());
  }

  SetCachedType(first_type);
  return first_type;
}

auto IfThenElse::Codegen(CompilationUnit &unit) const noexcept
    -> Outcome<llvm::Value *, Error> {}

void IfThenElse::Print(std::ostream &stream) const noexcept {
  stream << "if (" << test << ")" << first << " else " << second;
}
} // namespace pink
