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
#include "ast/Block.h"
#include "ast/Conditional.h"
#include "ast/While.h"

#include "aux/Environment.h"

namespace pink {
/* 1/24/2023
The type of a block is the type of it's last expression,
if and only if each expression within the block can be
typed. An empty block has type Nil.

block acts like a heterogeneous container, and therefore
does not place any additional constraints upon it's
data types.

#TODO: implement some sort of 'delay(expr)' builtin.
then we can build some lifetime annotations on top of
it. as well as some kind of dynamic memory cleanup builtin.
this intrinsic works at the block level, and lifetime
also needs to be accounted for at the expression level itself.
*/
auto Block::Typecheck(CompilationUnit &unit) const noexcept
    -> Outcome<Type::Pointer, Error> {
  if (IsEmpty()) {
    const auto *nil_type = unit.GetNilType();
    SetCachedType(nil_type);
    return nil_type;
  }

  Type::Pointer return_type = nullptr;

  unit.PushScope();
  for (const auto &expression : expressions) {
    auto expression_outcome = expression->Typecheck(unit);
    if (!expression_outcome) {
      unit.PopScope();
      return expression_outcome;
    }
    return_type = expression_outcome.GetFirst();
  }
  unit.PopScope();

  SetCachedType(return_type);
  return return_type;
}

/*
  simply emits the code for each expression in sequence.

  #TODO: emit llvm.lifetime.end(ptr) for each alloca
  created by the last codegen'ed expression that is
  temporary.
*/
auto Block::Codegen(CompilationUnit &unit) const noexcept
    -> Outcome<llvm::Value *, Error> {
  llvm::Value *result = nullptr;
  for (const auto &expression : expressions) {
    auto expression_outcome = expression->Codegen(unit);
    if (!expression_outcome) {
      return expression_outcome;
    }
    result = expression_outcome.GetFirst();
  }
  return result;
}

void Block::Print(std::ostream &stream) const noexcept {
  stream << "{";

  for (const auto &expression : expressions) {
    stream << expression;

    if (!llvm::isa<IfThenElse>(expression) && !llvm::isa<While>(expression)) {
      stream << "; ";
    } else {
      stream << " ";
    }
  }

  stream << "}";
}
} // namespace pink
