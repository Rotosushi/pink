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
#include "type/FunctionType.h"

#include "aux/Environment.h"

namespace pink {
auto FunctionType::ToLLVM(CompilationUnit &unit) const noexcept
    -> llvm::Type * {
  auto                            address_space  = unit.AllocaAddressSpace();
  std::size_t                     arguments_size = arguments.size() + 1;
  llvm::AttributeSet              function_attributes;
  llvm::AttributeSet              return_attributes;
  std::vector<llvm::AttributeSet> arguments_attributes{};
  arguments_attributes.reserve(arguments_size);
  std::vector<llvm::Type *> llvm_argument_types{};
  llvm_argument_types.reserve(arguments_size);

  auto *llvm_return_type = return_type->ToLLVM(unit);
  // #RULE if the return type of a function will not fit into a single
  // register we must pass it as a hidden parameter, we place it first in
  // the argument list. This parameter must be a pointer type, with the
  // sret<Ty> attribute, where Ty is the pointee type of the parameter
  // (the in memory return type).
  if (!llvm_return_type->isSingleValueType() && !llvm_return_type->isVoidTy()) {
    auto return_attributes_builder = unit.GetAttributeBuilder();
    return_attributes_builder.addStructRetAttr(llvm_return_type);
    arguments_attributes.emplace_back(
        unit.GetAttributeSet(return_attributes_builder));
    llvm_argument_types.emplace_back(unit.LLVMPointerType(address_space));
  }

  // #RULE we can only pass arguments which are single value types.
  // arguments which are larger than a single value type must
  // be passed via a pointer, and we must give these arguments
  for (const auto *argument : arguments) {
    auto *llvm_argument_type = argument->ToLLVM(unit);

    if (llvm_argument_type->isSingleValueType()) {
      llvm_argument_types.emplace_back(llvm_argument_type);
      arguments_attributes.emplace_back(unit.GetAttributeSet());
    } else {
      auto argument_attributes_builder = unit.GetAttributeBuilder();
      argument_attributes_builder.addByValAttr(llvm_argument_type);
      arguments_attributes.emplace_back(
          unit.GetAttributeSet(argument_attributes_builder));
      llvm_argument_types.emplace_back(unit.LLVMPointerType(address_space));
    }
  }

  SetAttributes(unit.GetAttributeList(function_attributes,
                                      return_attributes,
                                      arguments_attributes));

  auto *llvm_function_type =
      CompilationUnit::LLVMFunctionType(llvm_return_type, llvm_argument_types);
  SetCachedLLVMType(llvm_function_type);
  return llvm_function_type;
}

auto FunctionType::Equals(Type::Pointer right) const noexcept -> bool {
  const auto *other = llvm::dyn_cast<const FunctionType>(right);
  if (other == nullptr) {
    return false;
  }

  if (arguments.size() != other->arguments.size()) {
    return false;
  }

  auto cursor       = arguments.begin();
  auto other_cursor = other->arguments.begin();
  while (cursor != arguments.end()) {
    if (!(*cursor)->Equals(*other_cursor)) {
      return false;
    }

    cursor++;
    other_cursor++;
  }

  return return_type->Equals(other->return_type);
}

void FunctionType::Print(std::ostream &stream) const noexcept {
  stream << "fn (";
  std::size_t index  = 0;
  std::size_t length = arguments.size();
  for (const auto *type : arguments) {
    type->Print(stream);

    if (index < (length - 1)) {
      stream << ", ";
    }
    index++;
  }
  stream << ") -> ";
  return_type->Print(stream);
}
} // namespace pink
