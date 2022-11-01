
#include "ast/Application.h"

#include "aux/Environment.h"

#include "visitor/AstVisitor.h"

#include "support/Find.h"
#include "support/LLVMValueToString.h"

namespace pink {
Application::Application(const Location &location, std::unique_ptr<Ast> callee,
                         std::vector<std::unique_ptr<Ast>> arguments)
    : Ast(Ast::Kind::Application, location), callee(std::move(callee)),
      arguments(std::move(arguments)) {}

void Application::Accept(AstVisitor *visitor) const { visitor->Visit(this); }

auto Application::classof(const Ast *ast) -> bool {
  return ast->GetKind() == Ast::Kind::Application;
}

auto Application::ToString() const -> std::string {
  std::string result;
  result += callee->ToString();
  result += "(";

  size_t idx = 0;
  size_t len = arguments.size();
  for (const auto &arg : arguments) {
    result += arg->ToString();

    if (idx < (len - 1)) {
      result += ", ";
    }

    idx++;
  }
  result += ")";
  return result;
}

/*
 *  The type of an application is the return type of the
 *  callee, if and only if the number and types of each
 *  argument matches the number and type of the callee's
 *  arguments.
 *
 *        ENV |-
 *          c : T0 -> T1 -> ... -> Tn -> Tr
 *          a0 : Ta0, a1 : Ta1, ..., an : Tan,
 *          Ta0 = T0, Ta1 = T1, ..., Tan = Tn
 *   -------------------------------------------------
 *        ENV |- c a0 a1 ... an : Tr
 *
 *
 */
auto Application::TypecheckV(const Environment &env) const
    -> Outcome<Type *, Error> {
  auto calleeTy = callee->Typecheck(env);
  if (!calleeTy) {
    return calleeTy;
  }

  // #RULE we can only call objects with function type
  auto *calleeFnTy = llvm::dyn_cast<FunctionType>(calleeTy.GetFirst());
  if (calleeFnTy == nullptr) {
    Error error(Error::Code::TypeCannotBeCalled, GetLoc(),
                calleeTy.GetFirst()->ToString());
    return {error};
  }

  // check that the number of arguments matches
  if (arguments.size() != calleeFnTy->arguments.size()) {
    std::string errmsg = std::string("Function takes ") +
                         std::to_string(calleeFnTy->arguments.size()) +
                         " arguments, " + std::to_string(arguments.size()) +
                         " arguments were provided";
    return {Error(Error::Code::ArgNumMismatch, GetLoc(), errmsg)};
  }

  std::vector<Type *> argTys;
  // check that each argument can be typed
  for (const auto &arg : arguments) {
    Outcome<Type *, Error> outcome = arg->Typecheck(env);

    if (!outcome) {
      return {outcome.GetSecond()};
    }

    argTys.push_back(outcome.GetFirst());
  }

  // chech that each arguments type matches
  auto cmp = [](Type *left, Type *right) { return left != right; };

  auto mismatch = FindBetween(argTys.begin(), argTys.end(),
                              calleeFnTy->arguments.begin(), cmp);

  if (mismatch.first != argTys.end()) {
    std::string errmsg = std::string("expected arg type: ") +
                         (*mismatch.first)->ToString() +
                         ", actual arg type: " + (*mismatch.second)->ToString();
    return {Error(Error::Code::ArgTypeMismatch, GetLoc(), errmsg)};
  }

  // return the functions result type as the type of the application term.
  return {calleeFnTy->result};
}

// The procedure is largely identical to Typecheck, in that we simply codegen
// the callee and each argument, except for the fact that
// we have to call a few additional llvm functions to generate the correct
// code.
auto Application::Codegen(const Environment &env) const
    -> Outcome<llvm::Value *, Error> {
  assert(GetType() != nullptr);
  Outcome<llvm::Value *, Error> callee_result = callee->Codegen(env);

  if (!callee_result) {
    return {callee_result.GetSecond()};
  }

  auto *function = llvm::dyn_cast<llvm::Function>(callee_result.GetFirst());

  if (function == nullptr) {
    std::string errmsg =
        std::string("type was: ") + LLVMValueToString(callee_result.GetFirst());
    Error error(Error::Code::TypeCannotBeCalled, GetLoc(), errmsg);
    return {error};
  }

  std::vector<llvm::Value *> arg_values;

  for (const auto &arg : *this) {
    Outcome<llvm::Value *, Error> arg_result = arg->Codegen(env);

    if (!arg_result) {
      return {arg_result.GetSecond()};
    }
    assert(arg_result.GetFirst() != nullptr);
    arg_values.push_back(arg_result.GetFirst());
  }

  llvm::CallInst *call = nullptr;
  if (arg_values.empty()) {
    call = env.instruction_builder->CreateCall(llvm::FunctionCallee(function));
  } else {
    call = env.instruction_builder->CreateCall(llvm::FunctionCallee(function),
                                               arg_values);
  }

  call->setAttributes(function->getAttributes());

  return {call};
}

} // namespace pink
