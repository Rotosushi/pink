// Copyright (C) 2023 cadence
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
#include <algorithm>

#include "ast/action/Typecheck.h"

#include "ast/visitor/AstVisitor.h"
#include "visitor/VisitorResult.h"

#include "ast/All.h"

#include "aux/Environment.h"

/*
  These are semi experimental, thinking about how
  to implement a language feature which would give
  these semantics with a builtin type. i.e. rust Result<T> and ? operator
*/

#define TRY(outcome, variable, function, ...)                                  \
  auto outcome = function(__VA_ARGS__);                                        \
  if (!outcome) {                                                              \
    result = outcome.GetSecond();                                              \
    return;                                                                    \
  }                                                                            \
  [[maybe_unused]] auto *variable = outcome.GetFirst();

#define TRY_ELSE(outcome, variable, else_bb, function, ...)                    \
  auto outcome = function(__VA_ARGS__);                                        \
  if (!outcome) {                                                              \
    result = outcome.GetSecond();                                              \
    else_bb;                                                                   \
    return;                                                                    \
  }                                                                            \
  [[maybe_unused]] auto *variable = outcome.GetFirst();

namespace pink {
class TypecheckVisitor : public ConstVisitorResult<TypecheckVisitor,
                                                   const Ast::Pointer &,
                                                   TypecheckResult>,
                         public ConstAstVisitor {
private:
  CompilationUnit &env;

public:
  void Visit(const AddressOf *address_of) const noexcept override;
  void Visit(const Application *application) const noexcept override;
  void Visit(const Array *array) const noexcept override;
  void Visit(const Assignment *assignment) const noexcept override;
  void Visit(const Bind *bind) const noexcept override;
  void Visit(const Binop *binop) const noexcept override;
  void Visit(const Block *block) const noexcept override;
  void Visit(const Boolean *boolean) const noexcept override;
  void Visit(const IfThenElse *conditional) const noexcept override;
  void Visit(const Dot *dot) const noexcept override;
  void Visit(const Function *function) const noexcept override;
  void Visit(const Integer *integer) const noexcept override;
  void Visit(const Nil *nil) const noexcept override;
  void Visit(const Subscript *subscript) const noexcept override;
  void Visit(const Tuple *tuple) const noexcept override;
  void Visit(const Unop *unop) const noexcept override;
  void Visit(const ValueOf *value_of) const noexcept override;
  void Visit(const Variable *variable) const noexcept override;
  void Visit(const While *loop) const noexcept override;

  TypecheckVisitor(CompilationUnit &env) noexcept
      : env(env) {}
  ~TypecheckVisitor() noexcept override                    = default;
  TypecheckVisitor(const TypecheckVisitor &other) noexcept = default;
  TypecheckVisitor(TypecheckVisitor &&other) noexcept      = default;
  auto operator=(const TypecheckVisitor &other) noexcept
      -> TypecheckVisitor & = delete;
  auto operator=(TypecheckVisitor &&other) noexcept
      -> TypecheckVisitor & = delete;
};

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
void TypecheckVisitor::Visit(const AddressOf *address_of) const noexcept {
  env.WithinAddressOf(true);
  TRY_ELSE(right_result,
           right_type,
           env.WithinAddressOf(false),
           Compute,
           address_of->GetRight(),
           this)
  env.WithinAddressOf(false);

  auto *pointer_type = env.GetPointerType(right_type);
  address_of->SetCachedType(pointer_type);
  result = pointer_type;
}

/* 1/24/2023
  The type of and Ast::Application is the result type of the
  callee function if and only if each argument type present in
  the Ast::Application matches the corresponding argument type
  present in the type of the callee function.

  Function Types are always constant, runtime, literal, non-temporary types,
  they are implicitly function pointers.

  arguments are taken by value (which implies a copy),
  therefore the type can be in memory or literal,
  comptime or runtime, and temporary or not.

  as a special case, an in memory temporary type may be
  used directly, (without a copy), due to it's lifetime being
  restricted to the evaluation of the call operation.
  (we can consider this a move.)

  a constant actual argument type can only bind to a constant formal argument
  a mutable actual argument type can bind to a constant or mutable formal
  argument.
*/
void TypecheckVisitor::Visit(const Application *application) const noexcept {
  TRY(callee_result, callee_type, Compute, application->GetCallee(), this)

  const auto *function_type =
      llvm::dyn_cast<FunctionType>(callee_result.GetFirst());
  if (callee_type == nullptr) {
    result = Error(Error::Code::TypeCannotBeCalled,
                   application->GetLocation(),
                   function_type->ToString());
    return;
  }

  if (application->GetArguments().size() !=
      function_type->GetArguments().size()) {
    std::string errmsg  = "Function takes [";
    errmsg             += std::to_string(function_type->GetArguments().size());
    errmsg             += "] arguments; [";
    errmsg             += std::to_string(application->GetArguments().size());
    errmsg             += "] arguments were provided.";
    result =
        Error(Error::Code::ArgNumMismatch, application->GetLocation(), errmsg);
    return;
  }

  FunctionType::Arguments computed_argument_types;
  computed_argument_types.reserve(application->GetArguments().size());
  for (const auto &argument : *application) {
    TRY(agrument_result, argument_type, Compute, argument, this)
    computed_argument_types.emplace_back(argument_type);
  }

  auto mismatch = std::mismatch(function_type->begin(),
                                function_type->end(),
                                computed_argument_types.cbegin());

  if (mismatch.first != function_type->end()) {
    std::stringstream errmsg;
    errmsg << "Expected argument type [";
    errmsg << *mismatch.first;
    errmsg << "], Actual argument type [";
    errmsg << *mismatch.second;
    errmsg << "]";
    result = Error(Error::Code::ArgTypeMismatch,
                   application->GetLocation(),
                   std::move(errmsg).str());
    return;
  }

  const auto *result_type = function_type->GetReturnType();
  application->SetCachedType(result_type);
  result = result_type;
}

/* 1/24/2023
  The type of an Array is an ArrayType of the
  same length and type as the Array if and only if
  each element of the Array is the same type as the
  first element of the Array.

  an array is-a literal iff all it's elements are literals
  otherwise we construct an in-memory type.
  when we construct an array literal it is constant,
  when we construct an array in memory it is mutable,
  when we construct an array it is a temporary value.
  (it must be bound to something to have it's lifetime extended)
*/
void TypecheckVisitor::Visit(const Array *array) const noexcept {
  std::vector<Type::Pointer> computed_element_types;
  computed_element_types.reserve(array->GetElements().size());

  for (const auto &element : *array) {
    TRY(element_result, element_type, Compute, element, this)

    computed_element_types.emplace_back(element_type);

    if (element_type != computed_element_types[0]) {
      std::stringstream errmsg;
      errmsg << "Element type [";
      errmsg << element_type;
      errmsg << "] does not match type predicted by first element [";
      errmsg << computed_element_types[0];
      errmsg << "]";
      result = Error(Error::Code::ArrayMemberTypeMismatch,
                     element->GetLocation(),
                     std::move(errmsg).str());
      return;
    }
  }

  auto const *result_type =
      env.GetArrayType(array->GetElements().size(), computed_element_types[0]);
  array->SetCachedType(result_type);
  result = {result_type};
}

/* 1/24/2023
  The type of an assignment is the Type of it's left hand side,
  if and only if the right hand side is the same type

  the lhs of an assignment must be a mutable, in memory type
  (I think this implies runtime and non-temporary).
  the rhs can be comptime or runtime, literal or in memory, const or mutable,
  temporary or not.
*/
void TypecheckVisitor::Visit(const Assignment *assignment) const noexcept {
  env.OnTheLHSOfAssignment(true);
  TRY_ELSE(left_result,
           left_type,
           env.OnTheLHSOfAssignment(false),
           Compute,
           assignment->GetLeft(),
           this)
  env.OnTheLHSOfAssignment(false);

  TRY(right_result, right_type, Compute, assignment->GetRight(), this)

  if (left_type != right_type) {
    std::stringstream errmsg;
    errmsg << "left type [";
    errmsg << left_type;
    errmsg << "] does not match right type [";
    errmsg << right_type;
    errmsg << "]";
    result = Error(Error::Code::AssigneeTypeMismatch,
                   assignment->GetLocation(),
                   std::move(errmsg).str());
    return;
  }

  assignment->SetCachedType(right_type);
  result = right_type;
}

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
void TypecheckVisitor::Visit(const Bind *bind) const noexcept {
  auto bound = env.LookupLocalVariable(bind->GetSymbol());

  if (bound.has_value()) {
    std::stringstream errmsg;
    errmsg << "symbol [";
    errmsg << bind->GetSymbol();
    errmsg << "] is already bound to type [";
    errmsg << bound->Type();
    errmsg << "]";
    result = Error(Error::Code::NameAlreadyBoundInScope,
                   bind->GetLocation(),
                   std::move(errmsg).str());
    return;
  }

  env.WithinBindExpression(true);
  TRY_ELSE(affix_result,
           affix_type,
           env.WithinBindExpression(false),
           Compute,
           bind->GetAffix(),
           this)
  env.WithinBindExpression(false);
  env.BindVariable(bind->GetSymbol(), affix_type, nullptr);

  bind->SetCachedType(affix_type);
  result = affix_type;
}

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
void TypecheckVisitor::Visit(const Binop *binop) const noexcept {
  TRY(left_result, left_type, Compute, binop->GetLeft(), this)

  TRY(right_result, right_type, Compute, binop->GetRight(), this)

  auto optional_literal = env.LookupBinop(binop->GetOp());

  if (!optional_literal || optional_literal->Empty()) {
    std::string errmsg  = "Unknown binop [";
    errmsg             += ToString(binop->GetOp());
    errmsg             += "]";
    result = Error(Error::Code::UnknownBinop, binop->GetLocation(), errmsg);
    return;
  }

  auto literal = optional_literal.value();

  auto optional_implementation = literal.Lookup(left_type, right_type);
  if (!optional_implementation.has_value()) {
    std::stringstream errmsg;
    errmsg << "Could not find an implementation of [";
    errmsg << ToString(binop->GetOp());
    errmsg << "] given the types [";
    errmsg << left_type;
    errmsg << ", ";
    errmsg << right_type;
    errmsg << "]";
    result = Error(Error::Code::ArgTypeMismatch,
                   binop->GetLocation(),
                   std::move(errmsg).str());
    return;
  }
  auto implementation = optional_implementation.value();

  const auto *result_type = implementation.ReturnType();
  binop->SetCachedType(result_type);
  result = result_type;
}

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
void TypecheckVisitor::Visit(const Block *block) const noexcept {
  if (block->IsEmpty()) {
    const auto *nil_type = env.GetNilType();
    result               = nil_type;
    block->SetCachedType(nil_type);
    return;
  }

  env.PushScope();
  for (const auto &expression : *block) {
    result = Compute(expression, this);

    if (!result) {
      env.PopScope();
      return;
    }
  }
  env.PopScope();
}

/* 1/24/2023
  The type of an ast::Boolean is Boolean.

  an ast::Boolean is a temporary, comptime, constant, literal.
*/
void TypecheckVisitor::Visit(const Boolean *boolean) const noexcept {
  const auto *result_type = env.GetBoolType();
  boolean->SetCachedType(result_type);
  result = result_type;
}

/* 1/24/2023
  The type of a IfThenElse expression is the type of the first of it's
  alternative expressions if and only if the test expression has type
  Boolean, and both alternative expressions have the same type.

  The test expression can work using a boolean type that is const or mutable,
  in memory or a literal, temporary or non-temporary, comptime or runtime.

  the then and else blocks are blocks, and so they can be in memory or literal,
  const or mutable, temporary or non-temporary.
*/
void TypecheckVisitor::Visit(const IfThenElse *conditional) const noexcept {
  TRY(test_result, test_type, Compute, conditional->GetTest(), this)

  if (test_type != env.GetBoolType()) {
    std::stringstream errmsg;
    errmsg << "Test expression has type [";
    errmsg << test_type;
    errmsg << "] expected type [Boolean]";
    result = Error(Error::Code::CondTestExprTypeMismatch,
                   conditional->GetTest()->GetLocation(),
                   std::move(errmsg).str());
    return;
  }

  TRY(first_result, first_type, Compute, conditional->GetFirst(), this)
  TRY(second_result, second_type, Compute, conditional->GetSecond(), this)
  if (first_type != second_type) {
    std::stringstream errmsg;
    errmsg << "first type [";
    errmsg << first_type;
    errmsg << "] second type [";
    errmsg << second_type;
    errmsg << "]";
    result = Error(Error::Code::CondBodyExprTypeMismatch,
                   conditional->GetLocation(),
                   std::move(errmsg).str());
    return;
  }

  conditional->SetCachedType(second_type);
  result = first_type;
}

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
*/
void TypecheckVisitor::Visit(const Dot *dot) const noexcept {
  TRY(left_result, left_type, Compute, dot->GetLeft(), this)

  const auto *structure_type = llvm::dyn_cast<TupleType>(left_type);
  if (structure_type == nullptr) {
    std::stringstream errmsg;
    errmsg << "Left has type [";
    errmsg << left_type;
    errmsg << "] which is not an accessable type.";
    result = Error(Error::Code::DotLeftIsNotATuple,
                   dot->GetLeft()->GetLocation(),
                   std::move(errmsg).str());
    return;
  }

  auto *right = llvm::dyn_cast<Integer>(dot->GetRight().get());
  if (right == nullptr) {
    std::stringstream errmsg;
    errmsg << "Right [";
    errmsg << dot->GetRight();
    errmsg << "] is not an Integer literal.";
    result = Error(Error::Code::DotRightIsNotAnInt,
                   dot->GetRight()->GetLocation(),
                   std::move(errmsg).str());
    return;
  }

  auto index = static_cast<std::size_t>(right->GetValue());
  if (index > structure_type->GetElements().size()) {
    std::stringstream errmsg;
    errmsg << "Index [";
    errmsg << std::to_string(index);
    errmsg << "] is larger than the highest indexable element [";
    errmsg << std::to_string(structure_type->GetElements().size() - 1);
    errmsg << "]";
    result = Error(Error::Code::DotIndexOutOfRange,
                   dot->GetLocation(),
                   std::move(errmsg).str());
    return;
  }

  const auto *result_type = structure_type->GetElements()[index];
  dot->SetCachedType(result_type);
  result = result_type;
}

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
*/
void TypecheckVisitor::Visit(const Function *function) const noexcept {
  env.PushScope();

  std::vector<Type::Pointer> argument_types;
  argument_types.reserve(function->GetArguments().size());
  for (const auto &argument : *function) {
    env.BindVariable(argument.first, argument.second, nullptr);
    argument_types.emplace_back(argument.second);
  }

  TRY_ELSE(body_result,
           body_type,
           env.PopScope(),
           Compute,
           function->GetBody(),
           this)
  env.PopScope();

  const auto *result_type =
      env.GetFunctionType(body_type, std::move(argument_types));
  function->SetCachedType(result_type);
  result = result_type;
}

/*
  The type of an integer literal is Integer

  an Integer is a constant, temporary, literal
*/
void TypecheckVisitor::Visit(const Integer *integer) const noexcept {
  const auto *result_type = env.GetIntType();
  integer->SetCachedType(result_type);
  result = result_type;
}

/*
  The type of the nil literal is Nil.

  a nil is a constant, temporary, literal.
*/
void TypecheckVisitor::Visit(const Nil *nil) const noexcept {
  const auto *result_type = env.GetNilType();
  nil->SetCachedType(result_type);
  result = result_type;
}

/*
  The type of a subscript operation is the element_type of the
  subscriptable pointer being accessed if and only if the left
  expression has slice or array type, and the right expression
  has Integer type.

  the lhs of a subscript can be a literal or in memory, it can
  be constant or mutable, and it can be temporary or not.
  the rhs of a subscript can be a literal or in memory, it can
  be constant or mutable, and it can be temporary or not.

  the result type is dependent upon the type of the array.
*/
void TypecheckVisitor::Visit(const Subscript *subscript) const noexcept {
  TRY(right_result, right_type, Compute, subscript->GetRight(), this)

  if ((llvm::dyn_cast<IntegerType>(right_type)) == nullptr) {
    std::stringstream errmsg;
    errmsg << "Cannot use type [";
    errmsg << right_type;
    errmsg << "] as an index.";
    result = Error(Error::Code::SubscriptRightIsNotAnIndex,
                   subscript->GetRight()->GetLocation(),
                   std::move(errmsg).str());
    return;
  }

  // #TODO iff the index is known at compile type do the range check

  TRY(left_result, left_type, Compute, subscript->GetLeft(), this)

  auto element_result = [&]() -> TypecheckResult {
    if (const auto *array_type = llvm::dyn_cast<ArrayType>(left_type);
        array_type != nullptr) {
      return array_type->GetElementType();
    }

    if (const auto *slice_type = llvm::dyn_cast<SliceType>(left_type);
        slice_type != nullptr) {
      return slice_type->GetPointeeType();
    }

    std::stringstream errmsg;
    errmsg << "Cannot subscript type [";
    errmsg << left_type;
    errmsg << "]";
    return Error(Error::Code::SubscriptLeftIsNotSubscriptable,
                 subscript->GetLeft()->GetLocation(),
                 std::move(errmsg).str());
  }();
  if (!element_result) {
    result = element_result;
    return;
  }
  const auto *element_type = element_result.GetFirst();
  subscript->SetCachedType(element_type);
  result = element_type;
}

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
void TypecheckVisitor::Visit(const Tuple *tuple) const noexcept {
  std::vector<Type::Pointer> element_types;
  element_types.reserve(tuple->GetElements().size());

  for (const auto &element : *tuple) {
    TRY(element_result, element_type, Compute, element, this)
    element_types.emplace_back(element_type);
  }

  const auto *result_type = env.GetTupleType(std::move(element_types));
  tuple->SetCachedType(result_type);
  result = result_type;
}

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
void TypecheckVisitor::Visit(const Unop *unop) const noexcept {
  auto optional_literal = env.LookupUnop(unop->GetOp());
  if (!optional_literal) {
    std::stringstream errmsg;
    errmsg << "Unknown unop [";
    errmsg << ToString(unop->GetOp());
    errmsg << "]";
    result = Error(Error::Code::UnknownUnop,
                   unop->GetLocation(),
                   std::move(errmsg).str());
    return;
  }
  auto &literal = optional_literal.value();

  TRY(right_result, right_type, Compute, unop->GetRight(), this)

  auto found = literal.Lookup(right_type);

  if (!found) {
    std::stringstream errmsg;
    errmsg << "No implementation of unop [";
    errmsg << ToString(unop->GetOp());
    errmsg << "] found for type [";
    errmsg << right_type;
    errmsg << "]";
    result = Error(Error::Code::ArgTypeMismatch,
                   unop->GetLocation(),
                   std::move(errmsg).str());
    return;
  }
  auto implementation = found.value();

  const auto *result_type = implementation.ReturnType();
  unop->SetCachedType(result_type);
  result = result_type;
}

/*
  The type of Value of is T iff the right hand side has type
  Pointer<T>

  ValueOf can only dereference the pointer, (it's right hand side)
  iff that pointer is backed by some memory.
  the pointer may be const or mutable, temporary or not.
  because pointers only 'exist' at runtime, so does the type
  need to be runtime.
*/
void TypecheckVisitor::Visit(const ValueOf *value_of) const noexcept {
  env.WithinDereferencePtr(true);
  TRY_ELSE(right_result,
           right_type,
           env.WithinDereferencePtr(false),
           Compute,
           value_of->GetRight(),
           this)
  env.WithinDereferencePtr(false);

  if (const auto *pointer_type = llvm::dyn_cast<PointerType>(right_type);
      pointer_type != nullptr) {
    result = pointer_type->GetPointeeType();
  } else if (const auto *slice_type = llvm::dyn_cast<SliceType>(right_type);
             slice_type != nullptr) {
    result = slice_type->GetPointeeType();
  } else {
    std::stringstream errmsg;
    errmsg << "[";
    errmsg << right_type;
    errmsg << "]";
    result = Error{Error::Code::CannotDereferenceNonPointer,
                   value_of->GetLocation(),
                   std::move(errmsg).str()};
  }
}

/*
  The type of a variable is the type the variable is
  bound to in scope if and only if the type is bound
  in scope.

  the type annotations of a variable are completely dependent upon
  the annotations of the type it is bound to.
*/
void TypecheckVisitor::Visit(const Variable *variable) const noexcept {
  auto found = env.LookupVariable(variable->GetSymbol());

  if (found.has_value()) {
    variable->SetCachedType(found->Type());
    result = found->Type();
    return;
  }

  std::stringstream errmsg;
  errmsg << "[";
  errmsg << variable->GetSymbol();
  errmsg << "]";
  result = Error(Error::Code::NameNotBoundInScope,
                 variable->GetLocation(),
                 std::move(errmsg).str());
}

/*
  The type of a While loop is Nil if and only if the
  test expression has type Boolean and the body expression
  is typeable.

  the test expression can be comptime or runtime, const or mutable,
  temporary or not, and backed by memory or not. the body of
  the while loop acts like a new block of code.
*/
void TypecheckVisitor::Visit(const While *loop) const noexcept {
  TRY(test_result, test_type, Compute, loop->GetTest(), this)

  if (test_type != env.GetBoolType()) {
    std::stringstream errmsg;
    errmsg << "Test expression has type [";
    errmsg << test_type;
    errmsg << "] expected type [Boolean]";
    result = Error(Error::Code::WhileTestTypeMismatch,
                   loop->GetTest()->GetLocation(),
                   std::move(errmsg).str());
    return;
  }

  TRY(body_result, body_type, Compute, loop->GetBody(), this)

  const auto *result_type = env.GetNilType();
  loop->SetCachedType(result_type);
  result = result_type;
}

[[nodiscard]] auto Typecheck(const Ast::Pointer &ast,
                             CompilationUnit &env) noexcept -> TypecheckResult {
  auto cached_type = ast->GetCachedType();
  if (cached_type) {
    return cached_type.value();
  }

  TypecheckVisitor visitor(env);
  return visitor.Compute(ast, &visitor);
}
#undef TRY
#undef TRY_ELSE
} // namespace pink
