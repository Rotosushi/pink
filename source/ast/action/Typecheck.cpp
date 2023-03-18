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

#include "ast/action/Typecheck.h"
#include "ast/action/ToString.h"

#include "type/action/ToString.h"

#include "ast/visitor/AstVisitor.h"
#include "visitor/VisitorResult.h"

#include "ast/All.h"

#include "aux/Environment.h"

#include "support/Find.h"

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
  Environment &env;

public:
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
  void Visit(const Variable *variable) const noexcept override;
  void Visit(const While *loop) const noexcept override;

  TypecheckVisitor(Environment &env) noexcept
      : env(env) {}
  ~TypecheckVisitor() noexcept override                    = default;
  TypecheckVisitor(const TypecheckVisitor &other) noexcept = default;
  TypecheckVisitor(TypecheckVisitor &&other) noexcept      = default;
  auto operator=(const TypecheckVisitor &other) noexcept
      -> TypecheckVisitor & = delete;
  auto operator=(TypecheckVisitor &&other) noexcept
      -> TypecheckVisitor & = delete;
};

/* 1/24/2023
  The type of and Ast::Application is the result type of the
  callee function if and only if each argument type present in
  the Ast::Application matches the corresponding argument type
  present in the type of the callee function.
*/
void TypecheckVisitor::Visit(const Application *application) const noexcept {
  TRY(callee_result, callee_type, Compute, application->GetCallee(), this)

  const auto *function_type =
      llvm::dyn_cast<FunctionType>(callee_result.GetFirst());
  if (callee_type == nullptr) {
    result = Error(Error::Code::TypeCannotBeCalled,
                   application->GetLocation(),
                   ToString(function_type));
    return;
  }

  if (application->GetArguments().size() !=
      function_type->GetArguments().size()) {
    std::string errmsg = "Function takes [";
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

  auto cmp = [](Type::Pointer one, Type::Pointer two) { return one != two; };
  auto mismatch = FindBetween(function_type->begin(),
                              function_type->end(),
                              computed_argument_types.cbegin(),
                              cmp);

  if (mismatch.first != function_type->end()) {
    std::string errmsg = "Expected argument type [";
    errmsg             += ToString(*mismatch.first);
    errmsg             += "], Actual argument type [";
    errmsg             += ToString(*mismatch.second);
    errmsg             += "]";
    result =
        Error(Error::Code::ArgTypeMismatch, application->GetLocation(), errmsg);
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
*/
void TypecheckVisitor::Visit(const Array *array) const noexcept {
  std::vector<Type::Pointer> computed_element_types;
  computed_element_types.reserve(array->GetElements().size());

  for (const auto &element : *array) {
    TRY(element_result, element_type, Compute, element, this)

    computed_element_types.emplace_back(element_type);

    if (element_type != computed_element_types[0]) {
      std::string errmsg = "Element type [";
      errmsg             += ToString(element_type);
      errmsg += "] does not match type predicted by first element [";
      errmsg += ToString(computed_element_types[0]);
      errmsg += "]";
      result = Error(Error::Code::ArrayMemberTypeMismatch,
                     element->GetLocation(),
                     errmsg);
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
  if and only if the left hand side is the same type.
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
    std::string errmsg = "left type [";
    errmsg             += ToString(left_type);
    errmsg             += "] does not match right type [";
    errmsg             += ToString(right_type);
    errmsg             += "]";
    result             = Error(Error::Code::AssigneeTypeMismatch,
                   assignment->GetLocation(),
                   errmsg);
    return;
  }

  assignment->SetCachedType(right_type);
  result = right_type;
}

/* 1/24/2023
  The Type of a Ast::Bind is the type of it's right hand side,
  if and only if the name being bound is not already bound.
*/
void TypecheckVisitor::Visit(const Bind *bind) const noexcept {
  auto bound = env.LookupLocalVariable(bind->GetSymbol());

  if (bound.has_value()) {
    std::string errmsg = "symbol [";
    errmsg             += bind->GetSymbol();
    errmsg             += "] is already bound to type [";
    errmsg             += ToString(bound->Type());
    errmsg             += "]";
    result             = Error(Error::Code::NameAlreadyBoundInScope,
                   bind->GetLocation(),
                   errmsg);
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
*/
void TypecheckVisitor::Visit(const Binop *binop) const noexcept {
  TRY(left_result, left_type, Compute, binop->GetLeft(), this)

  TRY(right_result, right_type, Compute, binop->GetRight(), this)

  auto optional_literal = env.LookupBinop(binop->GetOp());

  if (!optional_literal || optional_literal->Empty()) {
    std::string errmsg = "Unknown binop [";
    errmsg             += binop->GetOp();
    errmsg             += "]";
    result = Error(Error::Code::UnknownBinop, binop->GetLocation(), errmsg);
    return;
  }

  auto literal = optional_literal.value();

  auto optional_implementation = literal.Lookup(left_type, right_type);
  if (!optional_implementation.has_value()) {
    std::string errmsg = "Could not find an implementation of [";
    errmsg             += binop->GetOp();
    errmsg             += "] given the types [";
    errmsg             += ToString(left_type);
    errmsg             += ", ";
    errmsg             += ToString(right_type);
    errmsg             += "]";
    result = Error(Error::Code::ArgTypeMismatch, binop->GetLocation(), errmsg);
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
  The type of a boolean literal is Boolean.
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
*/
void TypecheckVisitor::Visit(const IfThenElse *conditional) const noexcept {
  TRY(test_result, test_type, Compute, conditional->GetTest(), this)

  if (test_type != env.GetBoolType()) {
    std::string errmsg = "Test expression has type [";
    errmsg             += ToString(test_type);
    errmsg             += "] expected type [Boolean]";
    result             = Error(Error::Code::CondTestExprTypeMismatch,
                   conditional->GetTest()->GetLocation(),
                   errmsg);
    return;
  }

  TRY(first_result, first_type, Compute, conditional->GetFirst(), this)
  TRY(second_result, second_type, Compute, conditional->GetSecond(), this)

  if (first_type != second_type) {
    std::string errmsg = "first type [";
    errmsg             += ToString(first_type);
    errmsg             += "] second type [";
    errmsg             += ToString(second_type);
    errmsg             += "]";
    result             = Error(Error::Code::CondBodyExprTypeMismatch,
                   conditional->GetLocation(),
                   errmsg);
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
  structure types, though that might get dicey depending upon
  the semantics of unions. (which are useful and so we are going
  to try and have them, tagged i think, as that is safe and reasonably
  efficient)
*/
void TypecheckVisitor::Visit(const Dot *dot) const noexcept {
  TRY(left_result, left_type, Compute, dot->GetLeft(), this)

  const auto *structure_type = llvm::dyn_cast<TupleType>(left_type);
  if (structure_type == nullptr) {
    std::string errmsg = "Left has type [";
    errmsg             += ToString(left_type);
    errmsg             += "] which is not an accessable type.";
    result             = Error(Error::Code::DotLeftIsNotATuple,
                   dot->GetLeft()->GetLocation(),
                   errmsg);
    return;
  }

  // we cannot accept runtime values here, we could accept
  // a variable if it was a llvm::ConstantInt. and that opens
  // the door to comptime evaluation.
  // runtime values are disallowed because the return type
  // would have to be an implicit union type to be statically
  // semantically meaningful.
  // non owning reference to right
  auto *right = llvm::dyn_cast<Integer>(dot->GetRight().get());
  if (right == nullptr) {
    std::string errmsg = "Right [";
    errmsg             += ToString(dot->GetRight());
    errmsg             += "] is not an Integer literal.";
    result             = Error(Error::Code::DotRightIsNotAnInt,
                   dot->GetRight()->GetLocation(),
                   errmsg);
    return;
  }

  auto index = static_cast<std::size_t>(right->GetValue());
  if (index > structure_type->GetElements().size()) {
    std::string errmsg = "Index [";
    errmsg             += std::to_string(index);
    errmsg             += "] is larger than the highest indexable element [";
    errmsg += std::to_string(structure_type->GetElements().size() - 1);
    errmsg += "]";
    result = Error(Error::Code::DotIndexOutOfRange, dot->GetLocation(), errmsg);
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
*/
void TypecheckVisitor::Visit(const Function *function) const noexcept {

  env.PushScope();

  for (const auto &argument : *function) {
    env.BindVariable(argument.first, argument.second, nullptr);
  }

  TRY_ELSE(body_result,
           body_type,
           env.PopScope(),
           Compute,
           function->GetBody(),
           this)
  env.PopScope();

  std::vector<Type::Pointer> argument_types;
  argument_types.reserve(function->GetArguments().size());
  auto GetType = [](const Function::Argument &argument) {
    return argument.second;
  };
  std::transform(function->begin(),
                 function->end(),
                 argument_types.begin(),
                 GetType);

  const auto *result_type =
      env.GetFunctionType(body_type, std::move(argument_types));
  function->SetCachedType(result_type);
  result = result_type;
}

/*
  The type of an integer literal is Integer
*/
void TypecheckVisitor::Visit(const Integer *integer) const noexcept {
  const auto *result_type = env.GetIntType();
  integer->SetCachedType(result_type);
  result = result_type;
}

/*
  The type of the nil literal is Nil.
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
*/
void TypecheckVisitor::Visit(const Subscript *subscript) const noexcept {
  TRY(right_result, right_type, Compute, subscript->GetRight(), this)

  if ((llvm::dyn_cast<IntegerType>(right_type)) == nullptr) {
    std::string errmsg = "Cannot use type [";
    errmsg             += ToString(right_type);
    errmsg             += "] as an index.";
    result             = Error(Error::Code::SubscriptRightIsNotAnIndex,
                   subscript->GetRight()->GetLocation(),
                   errmsg);
    return;
  }

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

    std::string errmsg = "Cannot subscript type [";
    errmsg             += ToString(left_type);
    errmsg             += "]";
    return Error(Error::Code::SubscriptLeftIsNotSubscriptable,
                 subscript->GetLeft()->GetLocation(),
                 errmsg);
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
*/
void TypecheckVisitor::Visit(const Unop *unop) const noexcept {
  auto optional_literal = env.LookupUnop(unop->GetOp());
  if (!optional_literal) {
    std::string errmsg = "Unknown unop [";
    errmsg             += unop->GetOp();
    errmsg             += "]";
    result = Error(Error::Code::UnknownUnop, unop->GetLocation(), errmsg);
    return;
  }
  auto &literal = optional_literal.value();

  auto right_type_result = [&]() -> Outcome<Type::Pointer, Error> {
    if (strcmp(unop->GetOp(), "*") == 0) {
      env.WithinDereferencePtr(true);
      auto right_result = Compute(unop->GetRight(), this);
      env.WithinDereferencePtr(false);
      return right_result;
    }

    if (strcmp(unop->GetOp(), "&") == 0) {
      if (env.OnTheLHSOfAssignment()) {
        // we cannot assign to an address value.
        // that is like assignment to a number
        // we can assign to a variable holding
        // an address, but '&' creates a literal value.
        return Error(Error::Code::ValueCannotBeAssigned, unop->GetLocation());
      }
      env.WithinAddressOf(true);
      auto right_result = Compute(unop->GetRight(), this);
      env.WithinAddressOf(false);
      return right_result;
    }

    return Compute(unop->GetRight(), this);
  }();

  if (!right_type_result) {
    result = right_type_result;
    return;
  }
  const auto *right_type = right_type_result.GetFirst();

  auto found = literal.Lookup(right_type);

  if (!found) {
    std::string errmsg = "No implementation of unop [";
    errmsg             += unop->GetOp();
    errmsg             += "] found for type [";
    errmsg             += ToString(right_type);
    errmsg             += "]";
    result = Error(Error::Code::ArgTypeMismatch, unop->GetLocation(), errmsg);
    return;
  }
  auto implementation = found.value();

  const auto *result_type = implementation.ReturnType();
  unop->SetCachedType(result_type);
  result = result_type;
}

/*
  The type of a variable is the type the variable is
  bound to in scope if and only if the type is bound
  in scope.
*/
void TypecheckVisitor::Visit(const Variable *variable) const noexcept {
  auto found = env.LookupVariable(variable->GetSymbol());

  if (found.has_value()) {
    variable->SetCachedType(found->Type());
    result = found->Type();
    return;
  }

  std::string errmsg = "[";
  errmsg             += variable->GetSymbol();
  errmsg             += "]";
  result =
      Error(Error::Code::NameNotBoundInScope, variable->GetLocation(), errmsg);
}

/*
  The type of a While loop is Nil if and only if the
  test expression has type Boolean and the body expression
  is typeable.
*/
void TypecheckVisitor::Visit(const While *loop) const noexcept {
  TRY(test_result, test_type, Compute, loop->GetTest(), this)

  if (test_type != env.GetBoolType()) {
    std::string errmsg = "Test expression has type [";
    errmsg             += ToString(test_type);
    errmsg             += "]. expected type [Boolean]";
    result             = Error(Error::Code::WhileTestTypeMismatch,
                   loop->GetTest()->GetLocation(),
                   errmsg);
    return;
  }

  TRY(body_result, body_type, Compute, loop->GetBody(), this)

  const auto *result_type = env.GetNilType();
  loop->SetCachedType(result_type);
  result = result_type;
}

[[nodiscard]] auto Typecheck(const Ast::Pointer &ast, Environment &env) noexcept
    -> TypecheckResult {
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
