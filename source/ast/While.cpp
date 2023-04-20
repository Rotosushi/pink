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
#include "ast/While.h"

#include "aux/Environment.h"

namespace pink {
/*
  The type of a While loop is Nil if and only if the
  test expression has type Boolean and the body expression
  is typeable.

  the test expression can be comptime or runtime, const or mutable,
  temporary or not, and backed by memory or not. the body of
  the while loop acts like a new block of code.
*/
auto While::Typecheck(CompilationUnit &unit) const noexcept
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
    return Error(Error::Code::WhileTestTypeMismatch,
                 test->GetLocation(),
                 std::move(errmsg).str());
  }

  auto body_outcome = body->Typecheck(unit);
  if (!body_outcome) {
    return body_outcome;
  }

  const auto *result_type = unit.GetNilType();
  SetCachedType(result_type);
  return result_type;
}

auto While::Codegen(CompilationUnit &unit) const noexcept
    -> Outcome<llvm::Value *, Error> {}

void While::Print(std::ostream &stream) const noexcept {
  stream << "while (" << test << ") " << body;
}
} // namespace pink
