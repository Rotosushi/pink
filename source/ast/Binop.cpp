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
#include "ast/Binop.h"

#include "aux/Environment.h"

namespace pink {
/*
The type of a binop expression is the return type of it's
implementation given the types of it's left and right hand
side expressions if and only if there is an implementation
of the binop given the left and right expressions.

binop works like a function call in that the lhs and rhs types
may be const or mutable, in memory or a literal, temporary
or non-temporary, comptime or runtime.
the result type of a binop is dependent upon the result type of
the implementation of the binop.
*/
auto Binop::Typecheck(CompilationUnit &unit) const noexcept
    -> Outcome<Type::Pointer, Error> {
  auto left_outcome = left->Typecheck(unit);
  if (!left_outcome) {
    return left_outcome;
  }
  auto left_type = left_outcome.GetFirst();

  auto right_outcome = right->Typecheck(unit);
  if (!right_outcome) {
    return right_outcome;
  }
  auto right_type = right_outcome.GetFirst();

  auto optional_literal = unit.LookupBinop(op);
  if (!optional_literal || optional_literal->Empty()) {
    std::stringstream errmsg;
    errmsg << "Unknown binop [" << ToString(op) << "]";
    return Error(Error::Code::UnknownBinop,
                 GetLocation(),
                 std::move(errmsg).str());
  }
  auto literal = optional_literal.value();

  auto optional_implementation = literal.Lookup(left_type, right_type);
  if (!optional_implementation.has_value()) {
    std::stringstream errmsg;
    errmsg << "Could not find an implementation of [";
    errmsg << ToString(op);
    errmsg << "] given the types [";
    errmsg << left_type;
    errmsg << ", ";
    errmsg << right_type;
    errmsg << "]";
    return Error(Error::Code::ArgTypeMismatch,
                 GetLocation(),
                 std::move(errmsg).str());
  }
  auto implementation = optional_implementation.value();

  const auto *result_type = implementation.ReturnType();
  SetCachedType(result_type);
  return result_type;
}

/*
  Codegen the left hand side, the right hand side,
  then emit the implementation of the binop using 
  the values of the left and right.
*/
auto Binop::Codegen(CompilationUnit &unit) const noexcept
    -> Outcome<llvm::Value *, Error> {
  auto left_type      = left->GetCachedTypeOrAssert();
  auto right_type      = right->GetCachedTypeOrAssert();

  auto left_outcome = left->Codegen(unit);
  if (!left_outcome) {
    return left_outcome;
  }
  auto left_value = left_outcome.GetFirst();

  auto right_outcome = right->Codegen(unit);
  if (!right_outcome) {
    return right_outcome;
  }
  auto right_value = right_outcome.GetFirst();

  auto optional_binop = unit.LookupBinop(op);
  assert(optional_binop.has_value());
  auto &binop = optional_binop.value();

  auto optional_impl = binop.Lookup(left_type, right_type);
  assert(optional_impl.has_value());
  auto impl = optional_impl.value();

  return impl(left_value, right_value, unit);
}

void Binop::Print(std::ostream &stream) const noexcept {
  if (llvm::isa<Binop>(left)) {
    stream << "(" << left << ")";
  } else {
    stream << left;
  }

  stream << " " << ToString(op) << " ";

  if (llvm::isa<Binop>(right)) {
    stream << "(" << right << ")";
  } else {
    stream << right;
  }
}
} // namespace pink
