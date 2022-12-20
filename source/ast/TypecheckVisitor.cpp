#include "ast/TypecheckVisitor.h"

namespace pink {
void TypecheckVisitor::Visit(const Application *application) const {
  auto callee_result = Compute(application->callee.get(), *this);
  if (!callee_result) {
    Return(std::move(callee_result));
    return;
  }
  auto *callee_type = callee_result.GetFirst();

  // #RULE we can only call objects with function type
  auto *callee_function_type = llvm::dyn_cast<FunctionType>(callee_type);
  if (callee_function_type == nullptr) {
    Return(Error(Error::TypeCannotBeCalled, application->GetLoc(),
                 callee_type->ToString()));
    return;
  }

  // #RULE applications are only valid if the function takes exactly
  // as many arguments as the application term attempts to pass
  auto actual_size = application->arguments.size();
  auto formal_size = callee_function_type->arguments.size();
  if (actual_size != formal_size) {
    std::string errmsg = "Function takes [";
    errmsg += std::to_string(formal_size);
    errmsg += "] arguments. However [";
    errmsg += std::to_string(actual_size);
    errmsg += "] arguments are present.";
    Return(Error(Error::ArgNumMismatch, application->GetLoc(), errmsg));
    return;
  }

  // #RULE application terms are only typeable if each actual argument
  // is statically typeable
  std::vector<Type *> actual_argument_types;
  for (const auto &argument : application->arguments) {
    auto argument_type_result = Compute(argument.get(), *this);

    if (!argument_type_result) {
      Return(std::move(argument_type_result));
      return;
    }

    actual_argument_types.emplace_back(argument_type_result.GetFirst());
  }

  auto not_equal = [](Type *left, Type *right) { return left != right; };
  auto mismatch =
      FindBetween(actual_argument_types.begin(), actual_argument_types.end(),
                  callee_function_type->arguments.begin(), not_equal);
  // #RULE each given actual argument must have the same
  // type as the corresponding formal argument.
  if (mismatch.first != actual_argument_types.end()) {
    std::string errmsg = "Expected argument type [";
    errmsg += (*mismatch.first)->ToString();
    errmsg += "], However the actual argument type was [";
    errmsg += (*mismatch.second)->ToString();
    errmsg += "]";
    Return(Error(Error::ArgTypeMismatch, application->GetLoc(), errmsg));
    return;
  }

  // #RULE the Type of an application term is the result type of the callee.
  Return(callee_function_type->result);
}

void TypecheckVisitor::Visit(const Array *array) const {
  size_t index = 0;
  std::vector<Type *> element_types;
  for (const auto &element : array->members) {
    auto element_type_result = Compute(element.get(), *this);
    if (!element_type_result) {
      Return(element_type_result);
      return;
    }
    auto *element_type = element_type_result.GetFirst();

    element_types.emplace_back(element_type);

    // #RULE the static type of the first element of an array literal
    // is the deduced element type of the array, so each element of the
    // array must have an equivalent type
    if (element_type != element_types[0]) {
      std::string errmsg = "Element [";
      errmsg += std::to_string(index);
      errmsg += "] has type [";
      errmsg += element_type->ToString();
      errmsg += "] which does not match array's deduced type [";
      errmsg += element_types[0]->ToString();
      errmsg += "]";
      Return(Error(Error::ArrayMemberTypeMismatch, array->GetLoc(), errmsg));
      return;
    }

    index++;
  }
  // #RULE the type of an array is it's length and it's element type.
  Return(env.types->GetArrayType(element_types.size(), element_types[0]));
}

void TypecheckVisitor::Visit(const Assignment *assignment) const {
  // #RULE both sides of an assignment expression must have
  // static type
  auto left_type_result = Compute(assignment->GetLeft(), *this);
  if (!left_type_result) {
    Return(std::move(left_type_result));
    return;
  }
  auto *left_type = left_type_result.GetFirst();

  auto right_type_result = Compute(assignment->GetRight(), *this);
  if (!right_type_result) {
    Return(std::move(right_type_result));
    return;
  }
  auto *right_type = right_type_result.GetFirst();

  // #RULE The type of both sides of an assignment must be equal
  if (left_type != right_type) {
    std::string errmsg = "Type of the left hand side [";
    errmsg += left_type->ToString();
    errmsg += "] does not match the type of the right hand side [";
    errmsg += right_type->ToString();
    errmsg += "]";
    Return(Error(Error::AssigneeTypeMismatch, assignment->GetLoc(), errmsg));
    return;
  }

  Return(left_type);
}

void TypecheckVisitor::Visit(const Bind *bind) const {
  // #RULE: Bind only checks for symbol redefinition in the Local Scope
  // this allows for shadow declarations
  auto bound = env.bindings->LookupLocal(bind->symbol);
  if (bound.has_value()) {
    std::string errmsg = bind->symbol;
    errmsg += " is already bound in scope to type [";
    errmsg += bound->first->ToString();
    errmsg += "]";
    Return(Error(Error::NameAlreadyBoundInScope, bind->GetLoc(), errmsg));
    return;
  }

  // #RULE a bind expression is only statically typeable if it's right hand
  // side is statically typeable
  auto affix_type_result = Compute(bind->affix.get(), *this);
  if (!affix_type_result) {
    Return(std::move(affix_type_result));
    return;
  }
  auto *affix_type = affix_type_result.GetFirst();

  // Construct a false binding (or premature) to let
  // statements that occur later in the program know
  // about the type of this symbol.
  env.false_bindings->emplace_back(bind->symbol);
  env.bindings->Bind(bind->symbol, affix_type, nullptr);

  // #RULE the result type of a bind expression is the type of it's
  // right hand side.
  Return(affix_type);
}

void TypecheckVisitor::Visit(const Binop *binop) const {
  auto left_result = Compute(binop->left.get(), *this);
  if (!left_result) {
    Return(std::move(left_result));
    return;
  }
  auto *left_type = left_result.GetFirst();

  auto right_result = Compute(binop->right.get(), *this);
  if (!right_result) {
    Return(std::move(right_result));
    return;
  }
  auto *right_type = right_result.GetFirst();

  auto binop_literal = env.binops->Lookup(binop->op);
  // #RULE: any operator with zero overloads cannot be
  // used in an infix expression.
  if (!binop_literal.has_value() ||
      binop_literal->second->NumOverloads() == 0) {
    std::string errmsg = "Unknown binop [";
    errmsg += binop->op;
    errmsg += "]";
    Return(Error(Error::UnknownBinop, binop->GetLoc(), errmsg));
    return;
  }

  // #RULE binops are overloaded on their argument types
  auto overload = binop_literal->second->Lookup(left_type, right_type);

  if (!overload.has_value()) {
    std::string errmsg = "No overload available given types [";
    errmsg += left_type->ToString();
    errmsg += "] and [";
    errmsg += right_type->ToString();
    errmsg += "]";
    Return(Error(Error::ArgTypeMismatch, binop->GetLoc(), errmsg));
    return;
  }

  // #RULE the static type of a binop is the result type of
  // the binop expression.
  Return(overload->second->result_type);
}

void TypecheckVisitor::Visit(const Block *block) const {
  Outcome<Type *, Error> result;

  // #RULE each statement within a block must be
  // statically typeable.
  for (const auto &statement : block->statements) {
    result = Compute(statement.get(), *this);

    if (!result) {
      Return(result);
      return;
    }
  }

  // #RULE the type of a block is the type of it's last
  // statement.
  Return(result);
}

void TypecheckVisitor::Visit(const Bool *boolean) const {
  assert(boolean != nullptr);
  Return(env.types->GetBoolType());
}

void TypecheckVisitor::Visit(const Conditional *conditional) const {
  auto test_type_result = Compute(conditional->test.get(), *this);
  if (!test_type_result) {
    Return(std::move(test_type_result));
    return;
  }
  auto *test_type = test_type_result.GetFirst();

  if (test_type != env.types->GetBoolType()) {
    std::string errmsg = "Test expression has type [";
    errmsg += test_type->ToString();
    errmsg += "]";
    Return(Error(Error::CondTestExprTypeMismatch, conditional->test->GetLoc(),
                 errmsg));
    return;
  }

  auto first_result = Compute(conditional->first.get(), *this);
  if (!first_result) {
    Return(std::move(first_result));
    return;
  }
  auto *first_type = first_result.GetFirst();

  auto second_result = Compute(conditional->second.get(), *this);
  if (!second_result) {
    Return(std::move(second_result));
    return;
  }
  auto *second_type = second_result.GetFirst();

  // #RULE the alternative expressions of a conditional must
  // have identical type.
  if (first_type != second_type) {
    std::string errmsg = "First alternative has type [";
    errmsg += first_type->ToString();
    errmsg += "], Second Alternative has type [";
    errmsg += second_type->ToString();
    errmsg += "]";
    Return(
        Error(Error::CondBodyExprTypeMismatch, conditional->GetLoc(), errmsg));
    return;
  }

  // #RULE the type of a conditional expression is the type
  // of it's alternative expressions.
  Return(first_type);
}

void TypecheckVisitor::Visit(const Dot *dot) const {
  // #RULE both sides of a dot expression
  // must be statically typeable
  auto left_result = Compute(dot->left.get(), *this);
  if (!left_result) {
    Return(std::move(left_result));
    return;
  }
  auto *left_type = left_result.GetFirst();

  auto right_result = Compute(dot->right.get(), *this);
  if (!right_result) {
    Return(std::move(right_result));
    return;
  }
  auto *right_type = right_result.GetFirst();

  // #RULE the left type of a dot expression must
  // be a tuple type
  auto *tuple_type = llvm::dyn_cast<TupleType>(left_type);
  if (tuple_type == nullptr) {
    std::string errmsg = "Left has type [";
    errmsg += left_type->ToString();
    errmsg += "]";
    Return(Error(Error::DotLeftIsNotATuple, dot->GetLoc(), errmsg));
    return;
  }

  // #RULE the right hand side of a dot expression, accessing a
  // tuple must have IntegerType.
  auto *integer_type = llvm::dyn_cast<IntegerType>(right_type);
  if (integer_type == nullptr) {
    std::string errmsg = "Right has type [";
    errmsg += left_type->ToString();
    errmsg += "]";
    Return(Error(Error::DotRightIsNotAnInt, dot->GetLoc(), errmsg));
    return;
  }

  // #RULE The right hand side of the dot expression must be
  // knowable at compile time.
  const auto *index = llvm::dyn_cast<Int>(dot->GetRight());
  if (index == nullptr) {
    std::string errmsg = "Right is a [";
    errmsg += AstToString(dot->GetRight());
    errmsg += "]";
    Return(Error(Error::DotRightIsNotAnInt, dot->GetLoc(), errmsg));
    return;
  }

  // #RULE the index must be between 0 and the length of the tuple
  auto index_value = index->GetValue();
  auto tuple_size = static_cast<long long>(tuple_type->member_types.size());
  if (index_value >= tuple_size || index_value < 0) {
    std::string errmsg = "Tuple has [";
    errmsg += std::to_string(tuple_type->member_types.size());
    errmsg += "] elements, index is [";
    errmsg += std::to_string(index_value);
    errmsg += "]";
    Return(Error(Error::DotIndexOutOfRange, dot->GetLoc(), errmsg));
    return;
  }

  // #RULE the static type of a dot expression is the type
  // of the member of the tuple at the index given.
  Return(tuple_type->member_types[index_value]);
}

void TypecheckVisitor::Visit(const Function *function) const {
  auto local_env = Environment::NewLocalEnv(
      env, std::make_shared<SymbolTable>(env.bindings->OuterScope()));

  for (const auto &arg : function->arguments) {
    local_env->bindings->Bind(arg.first, arg.second, nullptr);
  }

  auto body_result = Compute(function->GetBody(), *this);

  for (const auto *binding : *(local_env->false_bindings)) {
    local_env->bindings->Unbind(binding);
  }
  local_env->false_bindings->clear();

  if (!body_result) {
    Return(std::move(body_result));
    return;
  }
  auto *body_type = body_result.GetFirst();

  std::vector<Type *> arg_types(function->GetArguments().size());

  auto to_type = [](const auto &arg) -> Type * { return arg.second; };
  std::transform(function->GetArguments().begin(),
                 function->GetArguments().end(), arg_types.begin(), to_type);
  auto *function_type = env.types->GetFunctionType(body_type, arg_types);

  env.false_bindings->emplace_back(function->GetName());
  env.bindings->Bind(function->GetName(), function_type, nullptr);

  Return(function_type);
}

void TypecheckVisitor::Visit(const Int *integer) const {
  assert(integer != nullptr);
  Return(env.types->GetIntType());
}

void TypecheckVisitor::Visit(const Nil *nil) const {
  assert(nil != nullptr);
  Return(env.types->GetNilType());
}

void TypecheckVisitor::Visit(const Subscript *subscript) const {
  auto left_result = Compute(subscript->GetLeft(), *this);
  if (!left_result) {
    Return(std::move(left_result));
    return;
  }
  auto *left_type = left_result.GetFirst();

  auto right_result = Compute(subscript->GetRight(), *this);
  if (!right_result) {
    Return(std::move(right_result));
    return;
  }
  auto *right_type = right_result.GetFirst();

  // #RULE the lhs of a subscript must be an array or a slice
  Type *element_type = nullptr;
  if (auto *array_type = llvm::dyn_cast<ArrayType>(left_type);
      array_type != nullptr) {
    element_type = array_type->member_type;
  } else if (auto *slice_type = llvm::dyn_cast<SliceType>(left_type);
             slice_type != nullptr) {
    element_type = slice_type->pointee_type;
  } else {
    std::string errmsg = "Cannot subscript type [";
    errmsg += left_type->ToString();
    errmsg += "]";
    Return(Error(Error::SubscriptLeftIsNotSubscriptable, subscript->GetLoc(),
                 errmsg));
    return;
  }

  // #RULE the rhs of a subscript must be an IntegerType
  auto *integer_type = llvm::dyn_cast<IntegerType>(right_type);
  if (integer_type == nullptr) {
    std::string errmsg = "Cannot use type [";
    errmsg += right_type->ToString();
    errmsg += "] as an index.";
    Return(
        Error(Error::SubscriptRightIsNotAnIndex, subscript->GetLoc(), errmsg));
    return;
  }

  // #RULE the static type of a subscript operation is
  // the element type of the array or slice being subscripted.
  Return(element_type);
}

void TypecheckVisitor::Visit(const Tuple *tuple) const {
  std::vector<Type *> member_types;

  for (const auto &element : tuple->members) {
    auto element_result = Compute(element.get(), *this);

    if (!element_result) {
      Return(std::move(element_result));
      return;
    }

    member_types.emplace_back(element_result.GetFirst());
  }

  Return(env.types->GetTupleType(member_types));
}

void TypecheckVisitor::Visit(const Unop *unop) const {
  auto right_result = Compute(unop->GetRight(), *this);
  if (!right_result) {
    Return(std::move(right_result));
    return;
  }
  auto *right_type = right_result.GetFirst();

  auto optional_unop = env.unops->Lookup(unop->GetOp());
  if (!optional_unop.has_value()) {
    std::string errmsg = "Unknown unop [";
    errmsg += unop->GetOp();
    errmsg += "]";
    Return(Error(Error::UnknownUnop, unop->GetLoc(), errmsg));
    return;
  }
  auto *unop_overloads = optional_unop->second;

  auto overload = [this, unop, right_type, unop_overloads]() {
    if (auto *pointer_type = llvm::dyn_cast<PointerType>(right_type);
        (pointer_type != nullptr) && (strcmp(unop->GetOp(), "*") == 0)) {
      // #RULE we can indirect any pointer type. (load the value)
      auto overload = unop_overloads->Lookup(right_type);

      if (!overload.has_value()) {
        auto *int_ptr_type = env.types->GetPointerType(env.types->GetIntType());
        auto indirection_unop = unop_overloads->Lookup(int_ptr_type);
        assert(indirection_unop.has_value() &&
               "Couldn't find integer pointer indirection unop");

        auto indirection_function = indirection_unop->second->generate;
        overload = unop_overloads->Register(
            pointer_type, pointer_type->pointee_type, indirection_function);
      }
      return overload;
    }

    if (strcmp(unop->GetOp(), "&") == 0) {
      // #RULE we can take the address of any well formed type.
      auto overload = unop_overloads->Lookup(right_type);

      if (!overload.has_value()) {
        auto *pointer_type = env.types->GetPointerType(right_type);
        auto *integer_type = env.types->GetIntType();
        auto address_of_integer = unop_overloads->Lookup(integer_type);
        assert(address_of_integer.has_value() &&
               "Couldn't find integer address of unop");

        auto address_of_function = address_of_integer->second->generate;
        overload = unop_overloads->Register(right_type, pointer_type,
                                            address_of_function);
      }
      return overload;
    }

    // #RULE the argument type must have a corresponding overload
    // within the unop's overload set.
    return unop_overloads->Lookup(right_type);
  }();

  // #RULE a unop with no overload for the actual argument type
  // cannot be statically typed.
  if (!overload.has_value()) {
    std::string errmsg = "Could not find an overload given the type [";
    errmsg += right_type->ToString();
    errmsg += "]";
    Return(Error(Error::ArgTypeMismatch, unop->GetLoc(), errmsg));
    return;
  }

  // #RULE the static type of a unop is the return type of
  // the selected overload.
  Return(overload->second->result_type);
}

void TypecheckVisitor::Visit(const Variable *variable) const {
  auto bound = env.bindings->Lookup(variable->GetSymbol());

  if (bound.has_value()) {
    Return(bound->first);
    return;
  }

  std::string errmsg = "Unknown symbol [";
  errmsg += variable->GetSymbol();
  errmsg += "]";
  Return(Error(Error::NameNotBoundInScope, variable->GetLoc(), errmsg));
}

void TypecheckVisitor::Visit(const While *loop) const {
  auto test_result = Compute(loop->GetTest(), *this);
  if (!test_result) {
    Return(std::move(test_result));
    return;
  }
  auto *test_type = test_result.GetFirst();

  // #RULE the test expression of a while loop must have type Boolean
  if (test_type != env.types->GetBoolType()) {
    std::string errmsg = "test expression has type [";
    errmsg += test_type->ToString();
    errmsg += "]";
    Return(
        Error(Error::WhileTestTypeMismatch, loop->GetTest()->GetLoc(), errmsg));
    return;
  }
  // #RULE A while loop cannot be statically typed if it's body
  // cannot be statically typed.
  auto body_result = Compute(loop->GetBody(), *this);
  if (!body_result) {
    Return(std::move(body_result));
    return;
  }
  // #RULE the static type of a while expression is Nil
  Return(env.types->GetNilType());
}

} // namespace pink