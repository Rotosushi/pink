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
    return Error(Error::Code::WhileTestTypeMismatch,
                 test->GetLocation(),
                 std::move(errmsg).str());
  }

  auto body_outcome = body->Typecheck(unit);
  if (!body_outcome) {
    return body_outcome;
  }

  // Nil literals are never in memory
  const auto *result_type = unit.GetNilType(annotations);
  SetCachedType(result_type);
  return result_type;
}

auto While::Codegen(CompilationUnit &unit) const noexcept
    -> Outcome<llvm::Value *> {
  auto *test_BB = unit.CreateAndInsertBasicBlock("test");
  auto *body_BB = unit.CreateBasicBlock("body");
  auto *end_BB  = unit.CreateBasicBlock("end");

  unit.CreateBr(test_BB);

  unit.SetInsertionPoint(test_BB);
  auto test_outcome = test->Codegen(unit);
  if (!test_outcome) {
    return test_outcome;
  }
  auto *test_value = test_outcome.GetFirst();
  assert(test_value != nullptr);
  unit.CreateCondBr(test_value, body_BB, end_BB);

  unit.SetInsertionPoint(body_BB);
  unit.InsertBasicBlock(body_BB);
  auto body_outcome = body->Codegen(unit);
  if (!body_outcome) {
    return body_outcome;
  }
  auto *body_value = body_outcome.GetFirst();
  assert(body_value != nullptr);
  unit.CreateBr(test_BB);

  unit.SetInsertionPoint(end_BB);
  unit.InsertBasicBlock(end_BB);

  return unit.ConstantBoolean(false);
}

void While::Print(std::ostream &stream) const noexcept {
  stream << "while (" << test << ") " << body;
}
} // namespace pink
