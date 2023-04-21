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
#include "ast/Bind.h"

#include "aux/Environment.h"

namespace pink {
/* 1/24/2023
  The Type of an Ast::Bind is the type of it's right hand side,
  if and only if the name being bound is not already bound.

  the rhs can be a literal or an in memory type.
  the rhs can be constant or mutable (as bind is a read only operation).
  the rhs can be temporary or non-temporary.
  the rhs can be runtime or comptime.

  bind always introduces a non-temporary type.
  bind introduces a constant, literal, comptime type iff the
  rhs is constant, literal, and comptime.
  bind introduces an in memory type iff the rhs is mutable,
  in-memory, or runtime.
  as a special case, iff the rhs is temporary and in-memory,
  then the bind expression does not allocate new memory or copy
  it simply 'steals'|'moves' the temporary allocation.
*/
auto Bind::Typecheck(CompilationUnit &unit) const noexcept
    -> Outcome<Type::Pointer, Error> {
  auto bound = unit.LookupLocalVariable(symbol);

  if (bound.has_value()) {
    std::stringstream errmsg;
    errmsg << "symbol [";
    errmsg << symbol;
    errmsg << "] is already bound to type [";
    errmsg << bound->Type();
    errmsg << "]";
    return Error(Error::Code::NameAlreadyBoundInScope,
                 GetLocation(),
                 std::move(errmsg).str());
  }

  unit.WithinBindExpression(true);
  auto affix_outcome = affix->Typecheck(unit);
  if (!affix_outcome) {
    unit.WithinBindExpression(false);
    return affix_outcome;
  }
  auto affix_type = affix_outcome.GetFirst();
  unit.WithinBindExpression(false);
  unit.BindVariable(symbol, affix_type, nullptr);

  SetCachedType(affix_type);
  return affix_type;
}

/*
  Introduce a new variable to the program.

  Bind Allocates space for a new variable with type T
  where T is the type of the affix expression.
*/
auto Bind::Codegen(CompilationUnit &unit) const noexcept
    -> Outcome<llvm::Value *, Error> {
  auto found = unit.LookupLocalVariable(symbol);
  assert(!found);

  auto affix_type = affix->GetCachedTypeOrAssert();

  unit.WithinBindExpression(true);
  auto affix_outcome = affix->Codegen(unit);
  if (!affix_outcome) {
    unit.WithinBindExpression(false);
    return affix_outcome;
  }
  unit.WithinBindExpression(false);
  auto affix_value = affix_outcome.GetFirst();

  // #RULE we must allocate stack space for singleValueType()s
  // as these types are not allocated when literals appear in
  // expressions.

  // #RULE We must not allocate space for non-singleValueType()s
  // as the Codegen function for non-singleValueType() literals
  // allocates stack space for them.
  auto llvm_type = ToLLVM(affix_type, unit);
  if (llvm_type->isSingleValueType()) {
    affix_value = unit.AllocateVariable(symbol, llvm_type, affix_value);
  }

  unit.BindVariable(symbol, affix_type, affix_value);
  return affix_value;
}

void Bind::Print(std::ostream &stream) const noexcept {
  stream << symbol << " := " << affix;
}
} // namespace pink
