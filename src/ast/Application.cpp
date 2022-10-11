
#include "ast/Application.h"

#include "aux/Environment.h"

#include "support/LLVMValueToString.h"

namespace pink {
Application::Application(const Location &location, std::unique_ptr<Ast> callee,
                         std::vector<std::unique_ptr<Ast>> arguments)
    : Ast(Ast::Kind::Application, location), callee(std::move(callee)),
      arguments(std::move(arguments)) {}

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
auto Application::GetypeV(const Environment &env) const
    -> Outcome<Type *, Error> {
  Outcome<Type *, Error> calleeTy = callee->Getype(env);

  if (!calleeTy) {
    return calleeTy;
  }

  // check that we have something we can call
  auto *calleeFnTy = llvm::dyn_cast<FunctionType>(calleeTy.GetFirst());

  if (calleeFnTy == nullptr) {
    Error error(Error::Code::TypeCannotBeCalled, GetLoc(),
                calleeTy.GetFirst()->ToString());
    return {error};
  }

  // check that the number of arguments matches
  if (arguments.size() != calleeFnTy->arguments.size()) {
    std::string errmsg = std::string("expected args: ") +
                         std::to_string(calleeFnTy->arguments.size()) +
                         ", actual args: " + std::to_string(arguments.size());
    Error error(Error::Code::ArgNumMismatch, GetLoc(), errmsg);
    return {error};
  }

  std::vector<Type *> argTys;
  // check that each argument can be typed
  for (const auto &arg : arguments) {
    Outcome<Type *, Error> outcome = arg->Getype(env);

    if (!outcome) {
      return {outcome.GetSecond()};
    }

    argTys.push_back(outcome.GetFirst());
  }

  // chech that each arguments type matches
  for (size_t idx = 0; idx < arguments.size(); idx++) {
    if (argTys[idx] != calleeFnTy->arguments[idx]) {
      std::string errmsg = std::string("expected arg type: ") +
                           calleeFnTy->arguments[idx]->ToString() +
                           ", actual arg type: " + argTys[idx]->ToString();
      Error error(Error::Code::ArgTypeMismatch, GetLoc(), errmsg);
      return {error};
    }
  }

  // return the functions result type as the type of the application term.
  return {calleeFnTy->result};
}

// The procedure is largely identical to Getype, in that we simply codegen
// the callee and each argument, except for the fact that
// we have to call a few additional llvm functions to generate the correct
// code.
auto Application::Codegen(const Environment &env) const
    -> Outcome<llvm::Value *, Error> {
  // 1: codegen callee to retrieve the function pointer
  Outcome<llvm::Value *, Error> callee_result = callee->Codegen(env);

  if (!callee_result) {
    return {callee_result.GetSecond()};
  }

  if (callee_result.GetFirst() == nullptr) {
    FatalError("Function implementation was empty!", __FILE__, __LINE__);
  }

  auto *function = llvm::dyn_cast<llvm::Function>(callee_result.GetFirst());

  if (function == nullptr) {
    std::string errmsg = std::string(", type was: ") +
                         LLVMValueToString(callee_result.GetFirst());
    Error error(Error::Code::TypeCannotBeCalled, GetLoc(), errmsg);
    return {error};
  }

  // 2: codegen each argument to retrieve each llvm::Value* being passed as
  //    argument
  std::vector<llvm::Value *> arg_values;

  for (const auto &arg : arguments) {
    Outcome<llvm::Value *, Error> arg_result = arg->Codegen(env);

    if (!arg_result) {
      return {arg_result.GetSecond()};
    }

    if (arg_result.GetFirst() == nullptr) {
      FatalError("The argument is nullptr!", __FILE__, __LINE__);
    }

    arg_values.push_back(arg_result.GetFirst());
  }

  llvm::CallInst *call = nullptr;
  // 3: codegen a CallInstruction to the callee passing in each argument.
  if (arg_values.empty()) {
    call = env.instruction_builder->CreateCall(llvm::FunctionCallee(function));
  } else {
    call = env.instruction_builder->CreateCall(llvm::FunctionCallee(function),
                                               arg_values);
  }
  // 4: return the result of the call instruction as the llvm::Value* of this
  //    procedure
  return {call};
}

} // namespace pink
