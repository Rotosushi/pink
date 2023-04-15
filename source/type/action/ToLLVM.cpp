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

#include "type/action/ToLLVM.h"

#include "type/All.h"

#include "aux/Environment.h"

#include "llvm/IR/DerivedTypes.h"

namespace pink {
class ToLLVMVisitor : public ConstVisitorResult<ToLLVMVisitor,
                                                const Type::Pointer,
                                                llvm::Type *>,
                      public ConstTypeVisitor {
private:
  CompilationUnit &env;

public:
  void Visit(const ArrayType *array_type) const noexcept override;
  void Visit(const BooleanType *boolean_type) const noexcept override;
  void Visit(const CharacterType *character_type) const noexcept override;
  void Visit(const FunctionType *function_type) const noexcept override;
  void Visit(const TypeVariable *identifier_type) const noexcept override;
  void Visit(const IntegerType *integer_type) const noexcept override;
  void Visit(const NilType *nil_type) const noexcept override;
  void Visit(const PointerType *pointer_type) const noexcept override;
  void Visit(const SliceType *slice_type) const noexcept override;
  void Visit(const TupleType *tuple_type) const noexcept override;
  void Visit(const VoidType *void_type) const noexcept override;

  ToLLVMVisitor(CompilationUnit &env) noexcept
      : ConstVisitorResult(),
        env(env) {}
  ~ToLLVMVisitor() noexcept override                 = default;
  ToLLVMVisitor(const ToLLVMVisitor &other) noexcept = default;
  ToLLVMVisitor(ToLLVMVisitor &&other) noexcept      = default;
  auto operator=(const ToLLVMVisitor &other) noexcept
      -> ToLLVMVisitor                                            & = delete;
  auto operator=(ToLLVMVisitor &&other) noexcept -> ToLLVMVisitor & = delete;
};

/*
  the layout of an ArrayType is a struct containing
  the size and the array allocation.
*/
void ToLLVMVisitor::Visit(const ArrayType *array_type) const noexcept {
  assert(array_type != nullptr);
  auto *llvm_element_type = Compute(array_type->GetElementType(), this);
  auto *llvm_type = env.LLVMArrayType(llvm_element_type, array_type->GetSize());
  array_type->SetCachedLLVMType(llvm_type);
  result = llvm_type;
}

/*
  the layout of a BooleanType is an integer of width 1.
*/
void ToLLVMVisitor::Visit(const BooleanType *boolean_type) const noexcept {
  auto *llvm_type = env.LLVMBooleanType();
  boolean_type->SetCachedLLVMType(llvm_type);
  result = llvm_type;
}

/*
  the layout of a CharacterType is an integer of width 8
*/
void ToLLVMVisitor::Visit(const CharacterType *character_type) const noexcept {
  auto *llvm_type = env.LLVMCharacterType();
  character_type->SetCachedLLVMType(llvm_type);
  result = llvm_type;
}

/*
  the layout of a FunctionType is a Function Pointer,

  We use this function to compute a baseline llvm::AttributeSet
  for any function which has this function type.
*/
void ToLLVMVisitor::Visit(const FunctionType *function_type) const noexcept {
  auto               address_space  = env.AllocaAddressSpace();
  std::size_t        arguments_size = function_type->GetArguments().size() + 1;
  llvm::AttributeSet function_attributes;
  llvm::AttributeSet return_attributes;
  std::vector<llvm::AttributeSet> arguments_attributes;
  arguments_attributes.reserve(arguments_size);
  std::vector<llvm::Type *> llvm_argument_types;
  llvm_argument_types.reserve(arguments_size);

  auto *llvm_return_type = Compute(function_type->GetReturnType(), this);
  // #RULE if the return type of a function will not fit into a single
  // register we must pass it as a hidden parameter, we place it first in
  // the argument list. This parameter must be a pointer type, with the
  // sret<Ty> attribute, where Ty is the pointee type of the parameter
  // (the in memory return type).
  if (!llvm_return_type->isSingleValueType() && !llvm_return_type->isVoidTy()) {
    auto return_attributes_builder = env.GetAttributeBuilder();
    return_attributes_builder.addStructRetAttr(llvm_return_type);
    arguments_attributes.emplace_back(
        env.GetAttributeSet(return_attributes_builder));
    llvm_argument_types.emplace_back(env.LLVMPointerType(address_space));
  }

  // #RULE we can only pass arguments which are single value types.
  // arguments which are larger than a single value type must
  // be passed via a pointer, and we must give these arguments
  for (const auto *argument : *function_type) {
    auto *llvm_argument_type = Compute(argument, this);

    if (llvm_argument_type->isSingleValueType()) {
      llvm_argument_types.emplace_back(llvm_argument_type);
      arguments_attributes.emplace_back(env.GetAttributeSet());
    } else {
      auto argument_attributes_builder = env.GetAttributeBuilder();
      argument_attributes_builder.addByValAttr(llvm_argument_type);
      arguments_attributes.emplace_back(
          env.GetAttributeSet(argument_attributes_builder));
      llvm_argument_types.emplace_back(env.LLVMPointerType(address_space));
    }
  }

  function_type->SetAttributes(env.GetAttributeList(function_attributes,
                                                    return_attributes,
                                                    arguments_attributes));

  auto *llvm_function_type =
      CompilationUnit::LLVMFunctionType(llvm_return_type, llvm_argument_types);
  function_type->SetCachedLLVMType(llvm_function_type);
  result = llvm_function_type;
}

void ToLLVMVisitor::Visit(const TypeVariable *identifier_type) const noexcept {
  auto found = env.LookupVariable(identifier_type->Identifier());
  assert(found);
  assert(found->Type() != nullptr);
  result = Compute(found->Type(), this);
}

/*
  The layout of an IntegerType is an integer of width 64
*/
void ToLLVMVisitor::Visit(const IntegerType *integer_type) const noexcept {
  auto *llvm_type = env.LLVMIntegerType();
  integer_type->SetCachedLLVMType(llvm_type);
  result = llvm_type;
}

/*
  The layout of a NilType is an integer of width 1
*/
void ToLLVMVisitor::Visit(const NilType *nil_type) const noexcept {
  auto *llvm_type = env.LLVMNilType();
  nil_type->SetCachedLLVMType(llvm_type);
  result = llvm_type;
}

/*
  The layout of a PointerType is a pointer type.
  (it's an integer type with width equal to a pointer
  type on the target architecture. Generally speaking
  it is safe to assume it is one word long, where word
  means smallest addressable unit of memory.)
*/
void ToLLVMVisitor::Visit(const PointerType *pointer_type) const noexcept {
  auto *llvm_type = env.LLVMPointerType();
  pointer_type->SetCachedLLVMType(llvm_type);
  result = llvm_type;
}

/*
  a Slice is a tuple of (size, offset, pointer)
*/
void ToLLVMVisitor::Visit(const SliceType *slice_type) const noexcept {
  auto *llvm_type = env.LLVMSliceType();
  slice_type->SetCachedLLVMType(llvm_type);
  result = llvm_type;
}

/*
  A tuple type is an anonymous structure type.
*/
void ToLLVMVisitor::Visit(const TupleType *tuple_type) const noexcept {
  std::vector<llvm::Type *> llvm_element_types(
      tuple_type->GetElements().size());
  auto transform_element = [&](Type::Pointer type) {
    return Compute(type, this);
  };
  std::transform(tuple_type->begin(),
                 tuple_type->end(),
                 llvm_element_types.begin(),
                 transform_element);
  auto *llvm_type = env.LLVMStructType(llvm_element_types);
  tuple_type->SetCachedLLVMType(llvm_type);
  result = llvm_type;
}

/*
  void type is void type
*/
void ToLLVMVisitor::Visit(const VoidType *void_type) const noexcept {
  auto *llvm_type = env.LLVMVoidType();
  void_type->SetCachedLLVMType(llvm_type);
  result = llvm_type;
}

[[nodiscard]] auto ToLLVM(Type::Pointer type, CompilationUnit &env) noexcept
    -> llvm::Type * {
  auto cache = type->CachedLLVMType();
  if (cache) {
    return cache.value();
  }

  ToLLVMVisitor visitor(env);
  return visitor.Compute(type, &visitor);
}
} // namespace pink
