#include "ast/action/Typecheck.h"
#include "ast/action/ToString.h"

#include "type/action/ToString.h"

#include "ast/visitor/AstVisitor.h"
#include "visitor/VisitorResult.h"

#include "ast/All.h"

#include "aux/Environment.h"

#include "support/Find.h"

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
  void Visit(const Conditional *conditional) const noexcept override;
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
  auto callee_result = Compute(application->GetCallee(), this);
  if (!callee_result) {
    result = callee_result;
    return;
  }
  auto *callee_type = llvm::dyn_cast<FunctionType>(callee_result.GetFirst());
  if (callee_type == nullptr) {
    result = Error(Error::Code::TypeCannotBeCalled,
                   application->GetLocation(),
                   ToString(callee_result.GetFirst()));
    return;
  }

  if (application->GetArguments().size() !=
      callee_type->GetArguments().size()) {
    std::string errmsg = "Function takes [";
    errmsg             += std::to_string(callee_type->GetArguments().size());
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
    auto argument_outcome = Compute(argument, this);
    if (!argument_outcome) {
      result = argument_outcome;
      return;
    }
    computed_argument_types.emplace_back(argument_outcome.GetFirst());
  }

  auto cmp = [](Type::Pointer one, Type::Pointer two) { return one != two; };
  auto mismatch = FindBetween(callee_type->begin(),
                              callee_type->end(),
                              computed_argument_types.cbegin(),
                              cmp);

  if (mismatch.first != callee_type->end()) {
    std::string errmsg = "Expected argument type [";
    errmsg             += ToString(*mismatch.first);
    errmsg             += "], Actual argument type [";
    errmsg             += ToString(*mismatch.second);
    errmsg             += "]";
    result =
        Error(Error::Code::ArgTypeMismatch, application->GetLocation(), errmsg);
    return;
  }

  auto *result_type = callee_type->GetReturnType();
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
    auto element_result = Compute(element, this);
    if (!element_result) {
      result = element_result;
      return;
    }
    auto *element_type = element_result.GetFirst();
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

  auto *result_type =
      env.type_interner.GetArrayType(array->GetElements().size(),
                                     computed_element_types[0]);
  array->SetCachedType(result_type);
  result = {result_type};
}

/* 1/24/2023
  The type of an assignment is the Type of it's left hand side,
  if and only if the left hand side is the same type.
*/
void TypecheckVisitor::Visit(const Assignment *assignment) const noexcept {
  env.internal_flags.OnTheLHSOfAssignment(true);
  auto left_outcome = Compute(assignment->GetLeft(), this);
  env.internal_flags.OnTheLHSOfAssignment(false);
  if (!left_outcome) {
    result = left_outcome;
    return;
  }
  auto *left_type = left_outcome.GetFirst();

  auto right_outcome = Compute(assignment->GetRight(), this);
  if (!right_outcome) {
    result = right_outcome;
    return;
  }
  auto *right_type = right_outcome.GetFirst();

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
  auto bound = env.scopes.LookupLocal(bind->GetSymbol());

  if (bound.has_value()) {
    std::string errmsg = "symbol [";
    errmsg             += bind->GetSymbol();
    errmsg             += "] is already bound to type [";
    errmsg             += ToString(bound->first);
    errmsg             += "]";
    result             = Error(Error::Code::NameAlreadyBoundInScope,
                   bind->GetLocation(),
                   errmsg);
    return;
  }

  env.internal_flags.WithinBindExpression(true);
  auto affix_result = Compute(bind->GetAffix(), this);
  if (!affix_result) {
    result = affix_result;
    return;
  }
  auto *affix_type = affix_result.GetFirst();
  env.internal_flags.WithinBindExpression(false);

  env.scopes.Bind(bind->GetSymbol(), affix_type, nullptr);

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
  auto left_result = Compute(binop->GetLeft(), this);
  if (!left_result) {
    result = left_result;
    return;
  }
  auto *left_type = left_result.GetFirst();

  auto right_result = Compute(binop->GetRight(), this);
  if (!right_result) {
    result = right_result;
    return;
  }
  auto *right_type = right_result.GetFirst();

  auto literal = env.binop_table.Lookup(binop->GetOp());

  if (!literal || literal->second->NumOverloads() == 0) {
    std::string errmsg = "Unknown binop [";
    errmsg             += binop->GetOp();
    errmsg             += "]";
    result = Error(Error::Code::UnknownBinop, binop->GetLocation(), errmsg);
    return;
  }

  auto implementation = literal->second->Lookup(left_type, right_type);
  if (!implementation.has_value()) {
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

  auto *result_type = implementation->second->GetReturnType();
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
    auto *nil_type = env.type_interner.GetNilType();
    result         = nil_type;
    block->SetCachedType(nil_type);
    return;
  }

  env.scopes.PushScope();
  for (const auto &expression : *block) {
    result = Compute(expression, this);

    if (!result) {
      env.scopes.PopScope();
      return;
    }
  }
  env.scopes.PopScope();
}

/* 1/24/2023
  The type of a boolean literal is Boolean.
*/
void TypecheckVisitor::Visit(const Boolean *boolean) const noexcept {
  auto *result_type = env.type_interner.GetBoolType();
  boolean->SetCachedType(result_type);
  result = result_type;
}

/* 1/24/2023
  The type of a Conditional expression is the type of the first of it's
  alternative expressions if and only if the test expression has type
  Boolean, and both alternative expressions have the same type.
*/
void TypecheckVisitor::Visit(const Conditional *conditional) const noexcept {
  auto test_result = Compute(conditional->GetTest(), this);
  if (!test_result) {
    result = test_result;
    return;
  }
  auto *test_type = test_result.GetFirst();

  if (test_type != env.type_interner.GetBoolType()) {
    std::string errmsg = "Test expression has type [";
    errmsg             += ToString(test_type);
    errmsg             += "] expected type [Boolean]";
    result             = Error(Error::Code::CondTestExprTypeMismatch,
                   conditional->GetTest()->GetLocation(),
                   errmsg);
    return;
  }

  auto first_result = Compute(conditional->GetFirst(), this);
  if (!first_result) {
    result = first_result;
    return;
  }
  auto *first_type = first_result.GetFirst();

  auto second_result = Compute(conditional->GetSecond(), this);
  if (!second_result) {
    result = second_result;
    return;
  }
  auto *second_type = second_result.GetFirst();

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
  auto left_result = Compute(dot->GetLeft(), this);
  if (!left_result) {
    result = left_result;
    return;
  }
  auto *left_type = left_result.GetFirst();

  auto *structure_type = llvm::dyn_cast<TupleType>(left_type);
  if (structure_type == nullptr) {
    std::string errmsg = "Left has type [";
    errmsg             += ToString(left_type);
    errmsg             += "] which is not an accessable type.";
    result             = Error(Error::Code::DotLeftIsNotATuple,
                   dot->GetLeft()->GetLocation(),
                   errmsg);
    return;
  }

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

  auto *result_type = structure_type->GetElements()[index];
  dot->SetCachedType(result_type);
  result = result_type;
}

/*
  The type of a function is it's FunctionType if and only if
  the body typechecks given that all of the function's arguments
  are bound to their respective types.
*/
void TypecheckVisitor::Visit(const Function *function) const noexcept {

  env.scopes.PushScope();

  for (const auto &argument : *function) {
    env.scopes.Bind(argument.first, argument.second, nullptr);
  }

  auto body_result = Compute(function->GetBody(), this);
  if (!body_result) {
    result = body_result;
    env.scopes.PopScope();
    return;
  }
  const auto *body_type = body_result.GetFirst();
  env.scopes.PopScope();

  std::vector<Type::Pointer> argument_types(function->GetArguments().size());
  auto                       GetType = [](const Function::Argument &argument) {
    return argument.second;
  };
  std::transform(function->begin(),
                 function->end(),
                 argument_types.begin(),
                 GetType);

  const auto *result_type =
      env.type_interner.GetFunctionType(body_type, argument_types);
  function->SetCachedType(result_type);
  result = result_type;
}

/*
  The type of an integer literal is Integer
*/
void TypecheckVisitor::Visit(const Integer *integer) const noexcept {
  const auto *result_type = env.type_interner.GetIntType();
  integer->SetCachedType(result_type);
  result = result_type;
}

/*
  The type of the nil literal is Nil.
*/
void TypecheckVisitor::Visit(const Nil *nil) const noexcept {
  const auto *result_type = env.type_interner.GetNilType();
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
  auto right_result = Compute(subscript->GetRight(), this);
  if (!right_result) {
    result = right_result;
    return;
  }
  const auto *right_type = right_result.GetFirst();

  if ((llvm::dyn_cast<IntegerType>(right_type)) == nullptr) {
    std::string errmsg = "Cannot use type [";
    errmsg             += ToString(right_type);
    errmsg             += "] as an index.";
    result             = Error(Error::Code::SubscriptRightIsNotAnIndex,
                   subscript->GetRight()->GetLocation(),
                   errmsg);
    return;
  }

  auto left_result = Compute(subscript->GetLeft(), this);
  if (!left_result) {
    result = left_result;
    return;
  }
  auto *left_type = left_result.GetFirst();

  auto element_result = [&]() -> TypecheckResult {
    if (auto *array_type = llvm::dyn_cast<ArrayType>(left_type);
        array_type != nullptr) {
      return array_type->GetElementType();
    }

    if (auto *slice_type = llvm::dyn_cast<SliceType>(left_type);
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
    auto element_result = Compute(element, this);
    if (!element_result) {
      result = element_result;
      return;
    }
    element_types.emplace_back(element_result.GetFirst());
  }

  const auto *result_type = env.type_interner.GetTupleType(element_types);
  tuple->SetCachedType(result_type);
  result = result_type;
}

/*
  The type of a Unop is the return type of the unops implementation
  given the type of it's right hand side expression as argument
  if and only if the unop has an implementation for the type of
  the right expression

  \todo refactor unary operator "*" and "&" to their own Ast nodes,
  just as dot "." is represented with 'Ast::Dot'.
  the reason being is their extra invariants; as of now "*" and "&"
  are being implemented within Ast::Unop. This is (I think) confusing
  the implementation of Ast::Unop, where it is doing more than simply
  checking the invariants of 'normal' unops. This is reflected (I think)
  in the implementation of "*" and "&", their codegen functions,
  which both simply return what they were passed without modification.
  The entire funcionality of the operation is just in changing how we
  treat the pointer to some allocation.
*/
void TypecheckVisitor::Visit(const Unop *unop) const noexcept {
  auto literal = env.unop_table.Lookup(unop->GetOp());
  if (!literal) {
    std::string errmsg = "Unknown unop [";
    errmsg             += unop->GetOp();
    errmsg             += "]";
    result = Error(Error::Code::UnknownUnop, unop->GetLocation(), errmsg);
    return;
  }

  auto implementation_result =
      [&]() -> Outcome<std::pair<Type::Pointer, UnopCodegen *>, Error> {
    if (strcmp(unop->GetOp(), "*") == 0) {
      env.internal_flags.WithinDereferencePtr(true);
      auto right_result = Compute(unop->GetRight(), this);
      env.internal_flags.WithinDereferencePtr(false);
      if (!right_result) {
        return right_result.GetSecond();
      }
      auto *right_type = right_result.GetFirst();
      // create a new instance of the dereference op
      // with the same body as every other dereference op.
      auto  dereference_integer =
          literal->second->Lookup(env.type_interner.GetIntType());
      assert(dereference_integer.has_value());
      return literal->second->Register(
          env.type_interner.GetPointerType(right_type),
          right_type,
          dereference_integer->second->GetGenerateFn());
    }

    if (strcmp(unop->GetOp(), "&") == 0) {
      if (env.internal_flags.OnTheLHSOfAssignment()) {
        // we cannot assign to an address value.
        // that is like assignment to a number
        return Error(Error::Code::ValueCannotBeAssigned, unop->GetLocation());
      }
      env.internal_flags.WithinAddressOf(true);
      auto right_result = Compute(unop->GetRight(), this);
      env.internal_flags.WithinAddressOf(false);
      if (!right_result) {
        return right_result.GetSecond();
      }
      auto *right_type = right_result.GetFirst();

      auto address_of_integer =
          literal->second->Lookup(env.type_interner.GetIntType());
      assert(address_of_integer.has_value());
      return literal->second->Register(
          right_type,
          env.type_interner.GetPointerType(right_type),
          address_of_integer->second->GetGenerateFn());
    }

    auto right_result = Compute(unop->GetRight(), this);
    if (!right_result) {
      return right_result.GetSecond();
    }
    auto *right_type = right_result.GetFirst();

    std::string errmsg = "No implementation of unop [";
    errmsg             += unop->GetOp();
    errmsg             += "] found for type [";
    errmsg             += ToString(right_type);
    errmsg             += "]";
    return Error(Error::Code::ArgTypeMismatch, unop->GetLocation(), errmsg);
  }();
  auto &implementation = implementation_result.GetFirst();

  const auto *result_type = implementation.second->GetReturnType();
  unop->SetCachedType(result_type);
  result = result_type;
}

/*
  The type of a variable is the type the variable is
  bound to in scope if and only if the type is bound
  in scope.
*/
void TypecheckVisitor::Visit(const Variable *variable) const noexcept {
  auto bound = env.scopes.Lookup(variable->GetSymbol());

  if (bound.has_value()) {
    variable->SetCachedType(bound->first);
    result = bound->first;
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
  auto test_result = Compute(loop->GetTest(), this);
  if (!test_result) {
    result = test_result;
    return;
  }
  const auto *test_type = test_result.GetFirst();

  if (test_type != env.type_interner.GetBoolType()) {
    std::string errmsg = "Test expression has type [";
    errmsg             += ToString(test_type);
    errmsg             += "]. expected type [Boolean]";
    result             = Error(Error::Code::WhileTestTypeMismatch,
                   loop->GetTest()->GetLocation(),
                   errmsg);
    return;
  }

  auto body_result = Compute(loop->GetBody(), this);
  if (!body_result) {
    result = body_result;
    return;
  }
  const auto *result_type = env.type_interner.GetNilType();
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
} // namespace pink
