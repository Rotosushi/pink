#include "type/action/ToLLVM.h"

#include "type/All.h"

#include "aux/Environment.h"

#include "llvm/IR/DerivedTypes.h"

namespace pink {
class ToLLVMVisitor : public ConstVisitorResult<ToLLVMVisitor,
                                                const TypeInterface::Pointer,
                                                llvm::Type *>,
                      public ConstTypeVisitor {
private:
  Environment &env;

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

  ToLLVMVisitor(Environment &env) noexcept
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

  for llvm we must promote any argument types which cannot
  fit into a single register to pointer types and then mark
  that argument with sret<Ty> where Ty is the original layout
  type of the argument. But the llvm::Function manages
  the sret<Ty> tag, so we cannot do that here.
  and we must promote the return type to a hidden first ptr type
  argument with the sret<Ty> tag if the return type is also not
  a single value type.
*/
void ToLLVMVisitor::Visit(const FunctionType *function_type) const noexcept {
  auto address_space = env.AllocaAddressSpace();

  std::vector<llvm::Type *> llvm_argument_types(
      function_type->GetArguments().size());

  auto transform_argument = [&](TypeInterface::Pointer type) -> llvm::Type * {
    llvm::Type *llvm_type = Compute(type, this);
    if (llvm_type->isSingleValueType()) {
      return llvm_type;
    }
    return env.LLVMPointerType(address_space);
  };

  std::transform(function_type->begin(),
                 function_type->end(),
                 llvm_argument_types.begin(),
                 transform_argument);

  auto *llvm_return_type = Compute(function_type->GetReturnType(), this);
  if (llvm_return_type->isSingleValueType() || llvm_return_type->isVoidTy()) {
    auto *llvm_type =
        env.LLVMFunctionType(llvm_return_type, llvm_argument_types, false);
    function_type->SetCachedLLVMType(llvm_type);
    result = llvm_type;
    return;
  }

  llvm_argument_types.insert(llvm_argument_types.begin(),
                             env.LLVMPointerType(address_space));
  auto *llvm_type =
      env.LLVMFunctionType(env.LLVMVoidType(), llvm_argument_types, false);
  function_type->SetCachedLLVMType(llvm_type);
  result = llvm_type;
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
  auto transform_element = [&](TypeInterface::Pointer type) {
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

[[nodiscard]] auto ToLLVM(TypeInterface::Pointer type,
                          Environment           &env) noexcept -> llvm::Type * {
  auto cache = type->CachedLLVMType();
  if (cache) {
    return cache.value();
  }

  ToLLVMVisitor visitor(env);
  return visitor.Compute(type, &visitor);
}
} // namespace pink
