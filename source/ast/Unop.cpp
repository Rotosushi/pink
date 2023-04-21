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
#include "ast/Unop.h"

#include "aux/Environment.h"

namespace pink {
/*
The type of a Unop is the return type of the unops implementation
given the type of it's right hand side expression as argument
if and only if the unop has an implementation for the type of
the right expression

like a function the arguments to a unop can be const or mutable,
they can be in-memory or literal, and they can be temporary or not.
the result type is dependent upon the result type of the
implementation of the unop.
*/
auto Unop::Typecheck(CompilationUnit &unit) const noexcept
    -> Outcome<Type::Pointer, Error> {
  auto optional_literal = unit.LookupUnop(op);
  if (!optional_literal) {
    std::stringstream errmsg;
    errmsg << "Unknown unop [";
    errmsg << ToString(op);
    errmsg << "]";
    return Error(Error::Code::UnknownUnop,
                 GetLocation(),
                 std::move(errmsg).str());
  }
  auto &literal = optional_literal.value();

  auto right_outcome = right->Typecheck(unit);
  if (!right_outcome) {
    return right_outcome;
  }
  auto right_type = right_outcome.GetFirst();

  auto found = literal.Lookup(right_type);

  if (!found) {
    std::stringstream errmsg;
    errmsg << "No implementation of unop [";
    errmsg << ToString(op);
    errmsg << "] found for type [";
    errmsg << right_type;
    errmsg << "]";
    return Error(Error::Code::ArgTypeMismatch,
                 right->GetLocation(),
                 std::move(errmsg).str());
  }
  auto implementation = found.value();

  const auto *result_type = implementation.ReturnType();
  SetCachedType(result_type);
  return result_type;
}

auto Unop::Codegen(CompilationUnit &unit) const noexcept
    -> Outcome<llvm::Value *, Error> {
  auto right_type = right->GetCachedTypeOrAssert();

  auto right_outcome = right->Codegen(unit);
  if (!right_outcome) {
    return right_outcome;
  }
  auto *right_value = right_outcome.GetFirst();

  auto optional_literal = unit.LookupUnop(op);
  assert(optional_literal.has_value());
  auto literal = optional_literal.value();

  auto optional_implementation = literal.Lookup(right_type);
  assert(optional_implementation.has_value());
  auto &implementation = optional_implementation.value();

  return implementation(right_value, unit);
}

void Unop::Print(std::ostream &stream) const noexcept {
  stream << ToString(op) << right;
}
} // namespace pink
