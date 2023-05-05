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
    -> Outcome<Type::Pointer> {
  auto test_outcome = test->Typecheck(unit);
  if (!test_outcome) {
    return test_outcome;
  }
  auto test_type = test_outcome.GetFirst();

  // #RULE we use the looser version of equality here
  // which does not check that annotations match.
  // yet we still need to provide annotations to construct
  // a type to compare to, so we somewhat arbitrarily select
  // a boolean literal type for comparison.
  Type::Annotations annotations;
  annotations.IsInMemory(false);
  if (!Equals(test_type, unit.GetBoolType(annotations))) {
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

/*
  We lower a conditional into a branch between two basic
  blocks, and a phi node which merges the two incoming
  values from the branches.

  #TODO: add if blocks without else branches. (these have Type Nil)
*/
auto IfThenElse::Codegen(CompilationUnit &unit) const noexcept
    -> Outcome<llvm::Value *> {
  // emit the test expression into whatever basic_block we
  // are currently inserting into.
  auto test_outcome = test->Codegen(unit);
  if (!test_outcome) {
    return test_outcome;
  }
  auto test_value = test_outcome.GetFirst();

  // #NOTE: 2/25/2023: We must not add the 'else' and 'merge'
  // basic blocks to the function upon creation, as the Codegen
  // for the first or second alternative may change which
  // basic block is last in the then or else block, and we want to append
  // exclusively to the end of the functions basic block list.
  auto *then_BB  = unit.CreateAndInsertBasicBlock("then");
  auto *else_BB  = unit.CreateBasicBlock("else");
  auto *merge_BB = unit.CreateBasicBlock("merge");

  unit.CreateCondBr(test_value, then_BB, else_BB);
  unit.SetInsertionPoint(then_BB);

  auto first_outcome = first->Codegen(unit);
  if (!first_outcome) {
    return first_outcome;
  }
  auto *first_value = first_outcome.GetFirst();
  assert(first_value != nullptr);

  unit.CreateBr(merge_BB);
  then_BB = unit.GetInsertionPoint().block;

  unit.InsertBasicBlock(else_BB);
  unit.SetInsertionPoint(else_BB);

  auto second_outcome = second->Codegen(unit);
  if (!second_outcome) {
    return second_outcome;
  }
  auto *second_value = second_outcome.GetFirst();
  assert(second_value != nullptr);

  unit.CreateBr(merge_BB);
  else_BB = unit.GetInsertionPoint().block;

  unit.InsertBasicBlock(merge_BB);
  unit.SetInsertionPoint(merge_BB);
  auto *phi = unit.CreatePHI(first_value->getType(), 2, "phi");
  phi->addIncoming(first_value, then_BB);
  phi->addIncoming(second_value, else_BB);
  return phi;
}

void IfThenElse::Print(std::ostream &stream) const noexcept {
  stream << "if (" << test << ")" << first << " else " << second;
}
} // namespace pink
