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
#include "ast/AddressOf.h"

#include "aux/Environment.h"

namespace pink {
/*
  Address of has the type Pointer<T> iff the right hand side has a type T

  Address of can only take the address if the type T is backed by memory.
  we cannot take the address of a literal.
  it can take the address of a const or mutable (the pointer is then const or
  mutable),
  and it can only take the address of a non-temporary value.
  the address of a stack allocated object is a constant, runtime, temporary,
  literal. the address of a function is a constant, runtime, non-temporary,
  literal.
*/
auto AddressOf::Typecheck(CompilationUnit &unit) const noexcept
    -> Outcome<Type::Pointer, Error> {
  unit.WithinAddressOf(true);
  auto right_outcome = right->Typecheck(unit);
  if (!right_outcome) {
    unit.WithinAddressOf(false);
    return right_outcome;
  }
  unit.WithinAddressOf(false);
  auto right_type = right_outcome.GetFirst();

  if (right_type->IsLiteral()) {
    std::stringstream errmsg;
    errmsg << "[" << right << "]";
    return Error{Error::Code::CannotTakeAddressOfLiteral,
                 GetLocation(),
                 errmsg.str()};
  }

  /*
    A pointer is the address of a value in memory.
    a pointer value can live in registers, so they
    are allowed to exist at runtime and not be
    in memory.
    The result of Address of is a Temporary,
    and is not constant. (this is so by default
    you can mutate a variable of address type.)
    the annotations of the pointee are taken directly
    from the type of the right hand side of Address of
  */
  Type::Annotations annotations;
  // annotations.IsTemporary(true);
  annotations.IsInMemory(false);
  // annotations.IsConstant(false);
  // annotations.IsComptime(false);
  auto pointer_type = unit.GetPointerType(annotations, right_type);
  SetCachedType(pointer_type);
  return pointer_type;
}

auto AddressOf::Codegen(CompilationUnit &unit) const noexcept
    -> Outcome<llvm::Value *, Error> {
  unit.WithinAddressOf(true);
  auto right_outcome = right->Codegen(unit);
  unit.WithinAddressOf(false);
  return right_outcome;
}

void AddressOf::Print(std::ostream &stream) const noexcept {
  stream << "&";
  stream << right;
}
} // namespace pink
