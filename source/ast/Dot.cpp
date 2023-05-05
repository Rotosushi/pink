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
#include "ast/Dot.h"
#include "ast/Integer.h"

#include "type/TupleType.h"

#include "aux/Environment.h"

namespace pink {
/* 1/24/2023
The Type of a Dot expression is the accessed member type
if and only if the left hand side expression has the type
Tuple and the right hand side expression is some Integer
literal whose value is within the range of the given Tuple.

#NOTE: when we add structure types, we will need to implement
named member access here. 1/24/2023

#ASIDE: theoretically we could support runtime variable indecies if
we computed the result type as the union of all of the available
structure types, though that might get dicey.

the left of a dot operator (the tuple) can be temporary or non-temporary,
const or mutable, and in memory or a literal, comptime or runtime.
the right (the index) needs to be a comptime value, it can be temporary
or not, it must be a literal (cannot reside in-memory), and it can be
const or mutable (though there is no way to construct )

the result type is dependent upon the type of the tuple.

the result type of a dot expression can be computed at runtime,
if we allow the result type to be a union of all of the types
within the tuple.
(if the types within a tuple of size N, each having a distinct type
then the union has N variants, where N is the number of unique types.
and a match statement can disambiguate the result of the dot expression
with no ambiguity of which member the value came from.
if two types within a tuple of size N are the same,
then the union has N - 1 variants.
and we can note that the union type itself is perfectly typesafe.
however one thing to note is that if the tuple has repeating types
then it becomes impossible to tell which member from the tuple you
are extracting. unless you know the index.
so if you were implementing
a loop over all elements within a tuple, you would have the index.
and since you are implementing the loop you have the full type of the
tuple.
well, by type yes. however we can still disambiguate via the index
  0 => case
  1 => case
  2 => case
  ...
  N => case

however that is a funny looking syntax, we could just skip the '.' operator
itself and give a special case to match statements for when the expression
being matched is itself a dot expression. then it would simply be up to
the programmer to choose the form which fit their usecase.
if you were printing the tuple, then you would already have the knowledge about
which member you were indexing.
I cannot think of a reason to index a tuple at runtime other than when you
want to handle each element of the tuple one after another in the same way.
that is, loop over the elements of a tuple as you would an array.
)
*/
auto Dot::Typecheck(CompilationUnit &unit) const noexcept
    -> Outcome<Type::Pointer> {
  auto left_outcome = left->Typecheck(unit);
  if (!left_outcome) {
    return left_outcome;
  }
  auto left_type = left_outcome.GetFirst();

  auto tuple_type = llvm::dyn_cast<TupleType>(left_type);
  if (tuple_type == nullptr) {
    std::stringstream errmsg;
    errmsg << "Left has type [";
    errmsg << left_type;
    errmsg << "] which is not an accessable type.";
    return Error(Error::Code::DotLeftIsNotATuple,
                 left->GetLocation(),
                 std::move(errmsg).str());
  }

  auto index = llvm::dyn_cast<Integer>(right.get());
  if (index == nullptr) {
    std::stringstream errmsg;
    errmsg << "Right of dot expression [";
    errmsg << right;
    errmsg << "] is not an Integer literal.";
    return Error(Error::Code::DotRightIsNotAnInt,
                 right->GetLocation(),
                 std::move(errmsg).str());
  }

  auto value = static_cast<std::size_t>(index->GetValue());
  if (value >= tuple_type->GetElements().size()) {
    std::stringstream errmsg;
    errmsg << "Index [";
    errmsg << std::to_string(value);
    errmsg << "] is larger than the highest indexable element [";
    errmsg << std::to_string(tuple_type->GetElements().size() - 1);
    errmsg << "]";
    return Error(Error::Code::DotIndexOutOfRange,
                 right->GetLocation(),
                 std::move(errmsg).str());
  }

  auto return_type = tuple_type->GetElements()[value];
  SetCachedType(return_type);
  return return_type;
}

auto Dot::Codegen(CompilationUnit &unit) const noexcept
    -> Outcome<llvm::Value *> {
  auto left_outcome = left->Codegen(unit);
  if (!left_outcome) {
    return left_outcome;
  }
  auto *left_value = left_outcome.GetFirst();
  assert(left_value != nullptr);
  auto left_type = ToLLVM(left->GetCachedTypeOrAssert(), unit);

  auto *index = llvm::dyn_cast<Integer>(right.get());
  assert(index != nullptr);
  auto *struct_type = llvm::cast<llvm::StructType>(left_type);
  auto *gep =
      unit.CreateConstInBoundsGEP2_32(struct_type,
                                      left_value,
                                      0,
                                      static_cast<unsigned>(index->GetValue()));
  auto *element_type =
      struct_type->getTypeAtIndex(static_cast<unsigned>(index->GetValue()));
  return unit.Load(element_type, gep);
}

void Dot::Print(std::ostream &stream) const noexcept {
  stream << left << "." << right;
}
} // namespace pink
