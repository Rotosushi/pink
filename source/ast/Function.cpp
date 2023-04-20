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
#include "ast/Function.h"

#include "aux/Environment.h"

namespace pink {
/*
The type of a function is it's FunctionType if and only if
the body typechecks given that all of the function's arguments
are bound to their respective types.

since this is the definition of a given function, and not an
operation, we don't need to impose any particular rules on
what types are allowed. (though this does not imply that the
programmer is disallowed from annotating the types present within
the function.)
the result type is dependent upon the type of the function body.

The result type of a function could be expressed as the union of
all of the types which the body returns. where union is equivalent
to the Algebraic product type.
then with a match statement you can match on the possible types
the function returns and handle each case. and since match can
check if the cases are exhaustive, you get a compile time error
if you don't handle all cases. and since duplicate return types
from a function are already considered to use the same memory
there can never be any Type ambiguity from the match statement.


this would be built on top of an is-a function which queries the
active type of a union, (union is-a T, union is-a U, etc...)
then we can dispatch over the type to the branch which handles
that type.

*/
auto Function::Typecheck(CompilationUnit &unit) const noexcept
    -> Outcome<Type::Pointer, Error> {
  unit.PushScope();

  std::vector<Type::Pointer> argument_types;
  argument_types.reserve(arguments.size());
  for (const auto &argument : arguments) {
    unit.BindVariable(argument.first, argument.second, nullptr);
    argument_types.emplace_back(argument.second);
  }

  auto body_outcome = body->Typecheck(unit);
  if (!body_outcome) {
    unit.PopScope();
    return body_outcome;
  }
  unit.PopScope();
  auto body_type = body_outcome.GetFirst();

  const auto *result_type =
      unit.GetFunctionType(body_type, std::move(argument_types));
  SetCachedType(result_type);
  return result_type;
}

/*

*/
auto Function::Codegen(CompilationUnit &unit) const noexcept
    -> Outcome<llvm::Value *, Error> {}

void Function::Print(std::ostream &stream) const noexcept {
  stream << "fn " << name << " (";

  std::size_t index  = 0;
  std::size_t length = arguments.size();
  for (const auto &argument : arguments) {
    stream << argument.first << ": " << argument.second;

    if (index < (length - 1)) {
      stream << ", ";
    }
    index++;
  }

  stream << ") " << body;
}
} // namespace pink
