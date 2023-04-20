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
#include "ast/Tuple.h"

#include "aux/Environment.h"

namespace pink {
/*
The type of a Tuple is a TupleType containing the type of
each of the elements of the Tuple if and only if each of
the elements of the Tuple Typecheck.

a tuple is a literal iff all it's components are also literal.
otherwise it is in-memory.
a tuple can be const or mutable, a literal is always const,
an in-memory can be either.
a tuple is always a temporary value. (it must be bound to something
to have it's lifetime extended.)
*/
auto Tuple::Typecheck(CompilationUnit &unit) const noexcept
    -> Outcome<Type::Pointer, Error> {
  std::vector<Type::Pointer> element_types;
  element_types.reserve(elements.size());

  for (const auto &element : elements) {
    auto element_outcome = element->Typecheck(unit);
    if (!element_outcome) {
      return element_outcome;
    }
    auto element_type = element_outcome.GetFirst();
    element_types.emplace_back(element_type);
  }

  const auto *result_type = unit.GetTupleType(std::move(element_types));
  SetCachedType(result_type);
  return result_type;
}

auto Tuple::Codegen(CompilationUnit &unit) const noexcept
    -> Outcome<llvm::Value *, Error> {}

void Tuple::Print(std::ostream &stream) const noexcept {
  stream << "(";

  std::size_t index  = 0;
  std::size_t length = elements.size();
  for (const auto &element : elements) {
    stream << element;

    if (index < (length - 1)) {
      stream << ", ";
    }
    index++;
  }

  stream << ")";
}
} // namespace pink
