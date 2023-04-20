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
#include "ast/Application.h"

#include "aux/Environment.h"

namespace pink {

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
auto Application::Typecheck(CompilationUnit &unit) const noexcept
    -> Outcome<Type::Pointer, Error> {
  auto callee_outcome = callee->Typecheck(unit);
  if (!callee_outcome) {
    return callee_outcome;
  }
  auto callee_type = callee_outcome.GetFirst();

  auto function_type = llvm::dyn_cast<FunctionType>(callee_type);
  if (function_type == nullptr) {
    std::stringstream stream;
    stream << function_type;
    return Error{Error::Code::TypeCannotBeCalled,
                 GetLocation(),
                 std::move(stream).str()};
  }

  if (arguments.size() != function_type->GetArguments().size()) {
    std::stringstream stream;
    stream << "Function takes [" << function_type->GetArguments().size();
    stream << "] arguments; [" << arguments.size()
           << "] arguments were provided.";
    return Error{Error::Code::ArgNumMismatch,
                 GetLocation(),
                 std::move(stream).str()};
  }

  FunctionType::Arguments actual_arguments;
  actual_arguments.reserve(arguments.size());
  for (const auto &argument : arguments) {
    auto argument_outcome = argument->Typecheck(unit);
    if (!argument_outcome) {
      return argument_outcome;
    }
    auto argument_type = argument_outcome.GetFirst();

    actual_arguments.emplace_back(argument_type);
  }

  auto actual_cursor = actual_arguments.begin();
  auto formal_cursor = function_type->GetArguments().begin();
  while (actual_cursor != actual_arguments.end()) {
    if (!Equals(*actual_cursor, *formal_cursor)) {
      std::stringstream errmsg;
      errmsg << "Expected argument type [";
      errmsg << *formal_cursor;
      errmsg << "], Actual argument type [";
      errmsg << *actual_cursor;
      errmsg << "]";
      return Error(Error::Code::ArgTypeMismatch,
                   GetLocation(),
                   std::move(errmsg).str());
    }

    actual_cursor++;
    formal_cursor++;
  }

  auto return_type = function_type->GetReturnType();
  SetCachedType(return_type);
  return return_type;
}

/*
  generate code for each actual argument,
  emit a call instruction specifying the callee,
  passing in the actual arguments.

*/
auto Application::Codegen(CompilationUnit &unit) const noexcept
    -> Outcome<llvm::Value *, Error> {
  auto callee_outcome = callee->Codegen(unit);
  if (!callee_outcome) {
    return callee_outcome;
  }
  auto callee_value = callee_outcome.GetFirst();
  auto function     = llvm::dyn_cast<llvm::Function>(callee_value);
  assert(function != nullptr);

  std::vector<llvm::Value *> actual_arguments;
  for (const auto &argument : arguments) {
    auto argument_outcome = argument->Codegen(unit);
    if (!argument_outcome) {
      return argument_outcome;
    }
    actual_arguments.emplace_back(argument_outcome.GetFirst());
  }

  auto call = [&]() {
    if (actual_arguments.empty()) {
      return unit.CreateCall(function);
    }
    return unit.CreateCall(function, actual_arguments);
  }();

  // does this set the extra call arguments on the emitted
  // call instruction?
  // i.e. adding in
  // 6. 'ty' the ... type of the return value
  // and/or
  // 7. 'fnty' ... the signature of the function called.
  // 9. 'function args' ....
  // 10. the optional function attributes list
  call->setAttributes(function->getAttributes());
  return call;
}

void Application::Print(std::ostream &stream) const noexcept {
  stream << callee;
  stream << "(";

  std::size_t index  = 0;
  std::size_t length = arguments.size();
  for (const auto &argument : arguments) {
    stream << argument;

    if (index < (length - 1)) {
      stream << ", ";
    }
    index++;
  }

  stream << ")";
}
} // namespace pink
