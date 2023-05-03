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
#include "ast/Function.h"

#include "aux/Environment.h"

#include "support/LLVMTypeToString.h"
#include "support/LLVMValueToString.h"

#include "llvm/IR/Verifier.h"

namespace pink {
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

The result type of a function could be expressed as the union of
all of the types which the body returns. where union is equivalent
to the Algebraic product type.
then with a match statement you can match on the possible types
the function returns and handle each case. and since match can
check if the cases are exhaustive, you get a compile time error
if you don't handle all cases. and since duplicate return types
from a function are already considered to use the same memory
there can never be any Type ambiguity from the match statement.


this would be built on top of an is-a function which queries the
active type of a union, (union is-a T, union is-a U, etc...)
then we can dispatch over the type to the branch which handles
that type.

*/
auto Function::Typecheck(CompilationUnit &unit) const noexcept
    -> Outcome<Type::Pointer, Error> {
  unit.PushScope();

  std::vector<Type::Pointer> argument_types;
  argument_types.reserve(arguments.size());
  for (const auto &argument : arguments) {
    unit.BindVariable(argument.first, argument.second, nullptr);
    argument_types.emplace_back(argument.second);
  }

  auto body_outcome = body->Typecheck(unit);
  if (!body_outcome) {
    unit.PopScope();
    return body_outcome;
  }
  unit.PopScope();
  auto body_type = body_outcome.GetFirst();

  // we can take the address of a function.
  Type::Annotations annotations;
  annotations.IsInMemory(true);
  const auto *result_type =
      unit.GetFunctionType(annotations, body_type, std::move(argument_types));
  SetCachedType(result_type);
  return result_type;
}

/*

*/
auto Function::Codegen(CompilationUnit &unit) const noexcept
    -> Outcome<llvm::Value *, Error> {
  auto is_main = [&]() {
    if (name[0] == 'm') {
      return strcmp(name, "main") == 0;
    }
    return false;
  }();

  const auto *cache_type         = GetCachedTypeOrAssert();
  const auto *pink_function_type = llvm::cast<FunctionType>(cache_type);
  auto        attributes         = pink_function_type->GetAttributes();

  auto *llvm_return_type   = ToLLVM(pink_function_type->GetReturnType(), unit);
  auto *llvm_function_type = [&]() {
    if (is_main) {
      // we can take the address of a function.
      Type::Annotations function_annotations;
      function_annotations.IsInMemory(true);
      Type::Annotations void_annotations;
      void_annotations.IsInMemory(false); // void types are never in memory
      const auto *main_function_type = unit.GetFunctionType(
          function_annotations,
          unit.GetVoidType(void_annotations),
          FunctionType::Arguments{pink_function_type->GetArguments()});
      pink_function_type = main_function_type;
      return llvm::cast<llvm::FunctionType>(main_function_type->ToLLVM(unit));
    }
    return llvm::cast<llvm::FunctionType>(pink_function_type->ToLLVM(unit));
  }();

  auto *llvm_function = unit.CreateFunction(llvm_function_type,
                                            llvm::Function::ExternalLinkage,
                                            name);

  llvm_function->setAttributes(attributes);

  auto *entry_BB = unit.CreateAndInsertBasicBlock(name + std::string("_entry"));
  unit.SetInsertionPoint(entry_BB);
  unit.PushScope();

  unit.ConstructFunctionArguments(llvm_function, this);

  auto body_outcome = body->Codegen(unit);
  if (!body_outcome) {
    return body_outcome;
  }
  auto *body_value = body_outcome.GetFirst();
  assert(body_value != nullptr);

  if (is_main) {
    unit.SysExit(body_value);
    unit.CreateRetVoid();
  } else {
    if (llvm_return_type->isSingleValueType()) {
      unit.CreateRet(body_value);
    } else {
      unit.Store(llvm_return_type, body_value, llvm_function->getArg(0));
    }
  }

  std::string              buffer;
  llvm::raw_string_ostream out(buffer);
  if (llvm::verifyFunction(*llvm_function, &out)) {
    out << " llvm function [\n"
        << LLVMValueToString(llvm_function) << "]\n"
        << "llvm type [" << LLVMTypeToString(llvm_function_type) << "]\n"
        << "pink type [" << pink_function_type->ToString() << "]\n"
        << "number of function attributes ["
        << llvm_function->getAttributes().getNumAttrSets() << "]\n";
    return Error(Error::Code::MalformedFunction, GetLocation(), buffer);
  }

  unit.LeaveCurrentFunction();
  unit.BindVariable(name, pink_function_type, llvm_function);
  return llvm_function;
}

void Function::Print(std::ostream &stream) const noexcept {
  stream << "fn " << name << " (";

  std::size_t index  = 0;
  std::size_t length = arguments.size();
  for (const auto &argument : arguments) {
    stream << argument.first << ": " << argument.second;

    if (index < (length - 1)) {
      stream << ", ";
    }
    index++;
  }

  stream << ") " << body;
}
} // namespace pink
