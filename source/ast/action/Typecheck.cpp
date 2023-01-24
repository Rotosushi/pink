#include "ast/action/Typecheck.h"
#include "type/action/ToString.h"

#include "ast/All.h"

#include "aux/Environment.h"

#include "support/Find.h"

namespace pink {

/*
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
    result = Error(Error::Code::TypeCannotBeCalled, application->GetLocation(),
                   ToString(callee_result.GetFirst()));
    return;
  }

  if (application->GetArguments().size() !=
      callee_type->GetArguments().size()) {
    std::string errmsg = "Function takes [";
    errmsg += std::to_string(callee_type->GetArguments().size());
    errmsg += "] arguments; [";
    errmsg += std::to_string(application->GetArguments().size());
    errmsg += "] arguments were provided.";
    result =
        Error(Error::Code::ArgNumMismatch, application->GetLocation(), errmsg);
    return;
  }

  std::vector<Type::Pointer> computed_argument_types;
  for (const auto &argument : *application) {
    auto argument_outcome = Compute(argument, this);
    if (!argument_outcome) {
      result = argument_outcome;
      return;
    }
    computed_argument_types.push_back(argument_outcome.GetFirst());
  }

  auto cmp = [](Type::Pointer one, Type::Pointer two) { return one != two; };
  auto mismatch = FindBetween(callee_type->begin(), callee_type->end(),
                              computed_argument_types.begin(), cmp);

  if (mismatch.first != callee_type->end()) {
    std::string errmsg = "Expected argument type [";
    errmsg += ToString(*mismatch.first);
    errmsg += "], Actual argument type [";
    errmsg += ToString(*mismatch.second);
    errmsg += "]";
    result =
        Error(Error::Code::ArgTypeMismatch, application->GetLocation(), errmsg);
    return;
  }

  auto *result_type = callee_type->GetReturnType();
  application->SetCachedType(result_type);
  result = result_type;
}

/*
  The type of an Array is an ArrayType of the
  same length and type as the Array if and only if
  each element of the Array is the same type as the
  first element of the Array.
*/
void TypecheckVisitor::Visit(const Array *array) const noexcept {
  std::vector<Type::Pointer> computed_element_types;

  for (const auto &element : *array) {
    auto element_result = Compute(element, this);
    if (!element_result) {
      result = element_result;
      return;
    }
    auto *element_type = element_result.GetFirst();
    computed_element_types.push_back(element_type);

    if (element_type != computed_element_types[0]) {
      std::string errmsg = "Element type [";
      errmsg += ToString(element_type);
      errmsg += "] does not match type predicted by first element [";
      errmsg += ToString(computed_element_types[0]);
      errmsg += "]";
      result = Error(Error::Code::ArrayMemberTypeMismatch,
                     element->GetLocation(), errmsg);
      return;
    }
  }

  auto *result_type = env.types->GetArrayType(array->GetElements().size(),
                                              computed_element_types[0]);
  array->SetCachedType(result_type);
  result = {result_type};
}

/*
  The type of an assignment is the Type of it's left hand side,
  if and only if the left hand side is the same type.
*/
void TypecheckVisitor::Visit(const Assignment *assignment) const noexcept {
  auto left_outcome = Compute(assignment->GetLeft(), this);
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
    errmsg += ToString(left_type);
    errmsg += "] does not match right type [";
    errmsg += ToString(right_type);
    errmsg += "]";
    result = Error(Error::Code::AssigneeTypeMismatch, assignment->GetLocation(),
                   errmsg);
    return;
  }

  assignment->SetCachedType(right_type);
  result = right_type;
}

/*
  The Type of a Ast::Bind is the type of it's right hand side,
  if and only if the name being bound is not already bound.
*/
void TypecheckVisitor::Visit(const Bind *bind) const noexcept {
  auto bound = env.bindings->LookupLocal(bind->GetSymbol());

  if (bound.has_value()) {
    std::string errmsg = "symbol [";
    errmsg += bind->GetSymbol();
    errmsg += "] is already bound to type [";
    errmsg += ToString(bound->first);
    errmsg += "]";
    result = Error(Error::Code::NameAlreadyBoundInScope, bind->GetLocation(),
                   errmsg);
    return;
  }

  auto affix_result = Compute(bind->GetAffix(), this);
  if (!affix_result) {
    result = affix_result;
    return;
  }
  auto *affix_type = affix_result.GetFirst();

  env.false_bindings->push_back(bind->GetSymbol());
  env.bindings->Bind(bind->GetSymbol(), affix_type, nullptr);

  bind->SetCachedType(affix_type);
  result = affix_type;
}

void TypecheckVisitor::Visit(const Binop *binop) const noexcept {
  auto left_result = Compute(binop->GetLeft(), this);
  if (!left_result) {
    result = left_result;
    return;
  }
  auto left_type = left_result.GetFirst();

  auto right_result = Compute(binop->GetRight(), this);
  if (!right_result) {
    result = right_result;
    return;
  }
  auto right_type = right_result.GetFirst();

  auto literal = env.binops->Lookup(binop->GetOp());

  if (!literal || literal->second->NumOverloads() == 0) {
    std::string errmsg = "Unknown binop [";
    errmsg += binop->GetOp();
    errmsg += "]";
    result = Error(Error::Code::UnknownBinop, binop->GetLocation(), errmsg);
    return;
  }

  auto implementation = literal->second->Lookup(left_type, right_type);
  if (!implementation.has_value()) {
    std::string errmsg = "Could not find an implementation of [";
    errmsg += binop->GetOp();
    errmsg += "] given the types [";
    errmsg += ToString(left_type);
    errmsg += ", ";
    errmsg += ToString(right_type);
    errmsg += "]";
    result = Error(Error::Code::ArgTypeMismatch, binop->GetLocation(), errmsg);
    return;
  }

  auto result_type = implementation->second->result_type;
  binop->SetCachedType(result_type);
  result = result_type;
}

void TypecheckVisitor::Visit(const Block *block) const noexcept {}

void TypecheckVisitor::Visit(const Boolean *boolean) const noexcept {}

void TypecheckVisitor::Visit(const Conditional *conditional) const noexcept {}

void TypecheckVisitor::Visit(const Dot *dot) const noexcept {}

void TypecheckVisitor::Visit(const Function *function) const noexcept {}

void TypecheckVisitor::Visit(const Integer *integer) const noexcept {}

void TypecheckVisitor::Visit(const Nil *nil) const noexcept {}

void TypecheckVisitor::Visit(const Subscript *subscript) const noexcept {}

void TypecheckVisitor::Visit(const Tuple *tuple) const noexcept {}

void TypecheckVisitor::Visit(const Unop *unop) const noexcept {}

void TypecheckVisitor::Visit(const Variable *variable) const noexcept {}

void TypecheckVisitor::Visit(const While *loop) const noexcept {}

[[nodiscard]] auto Typecheck(const Ast::Pointer &ast, Environment &env) noexcept
    -> TypecheckResult {
  TypecheckVisitor visitor(env);
  return TypecheckVisitor::Compute(ast, &visitor);
}
} // namespace pink
