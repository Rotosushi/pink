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
  Environment &env;

public:
  void Visit(const ArrayType *array_type) const noexcept override;
  void Visit(const BooleanType *boolean_type) const noexcept override;
  void Visit(const CharacterType *character_type) const noexcept override;
  void Visit(const FunctionType *function_type) const noexcept override;
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
  auto *llvm_integer_type = Compute(env.type_interner.GetIntType(), this);
  result                  = llvm::StructType::get(*env.context,
                                                  {llvm_integer_type, llvm_element_type});
}

/*
  the layout of a BooleanType is an integer of width 1.
*/
void ToLLVMVisitor::Visit(const BooleanType *boolean_type) const noexcept {
  assert(boolean_type != nullptr);
  result = env.instruction_builder->getInt1Ty();
}

/*
  the layout of a CharacterType is an integer of width 8
*/
void ToLLVMVisitor::Visit(const CharacterType *character_type) const noexcept {
  assert(character_type != nullptr);
  result = env.instruction_builder->getInt8Ty();
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
  assert(function_type != nullptr);
  auto address_space = env.module->getDataLayout().getAllocaAddrSpace();
  std::vector<llvm::Type *> llvm_argument_types(
      function_type->GetArguments().size());

  auto transform_argument = [&](Type::Pointer type) -> llvm::Type * {
    llvm::Type *llvm_type = Compute(type, this);
    if (llvm_type->isSingleValueType()) {
      return llvm_type;
    }
    return env.instruction_builder->getPtrTy(address_space);
  };

  std::transform(function_type->begin(),
                 function_type->end(),
                 llvm_argument_types.begin(),
                 transform_argument);

  auto *llvm_return_type = Compute(function_type->GetReturnType(), this);
  if (llvm_return_type->isSingleValueType() || llvm_return_type->isVoidTy()) {
    result =
        llvm::FunctionType::get(llvm_return_type, llvm_argument_types, false);
    return;
  }

  llvm_argument_types.insert(llvm_argument_types.begin(),
                             env.instruction_builder->getPtrTy(address_space));
  result = llvm::FunctionType::get(env.instruction_builder->getVoidTy(),
                                   llvm_argument_types,
                                   false);
}

/*
  The layout of an IntegerType is an integer of width 64
*/
void ToLLVMVisitor::Visit(const IntegerType *integer_type) const noexcept {
  assert(integer_type != nullptr);
  result = env.instruction_builder->getInt64Ty();
}

/*
  The layout of a NilType is an integer of width 1
*/
void ToLLVMVisitor::Visit(const NilType *nil_type) const noexcept {
  assert(nil_type != nullptr);
  result = env.instruction_builder->getInt1Ty();
}

/*
  The layout of a PointerType is a pointer type.
  (it's an integer type with width equal to a pointer
  type on the target architecture. Generally speaking
  it is safe to assume it is one word long, where word
  means smallest addressable unit of memory.)
*/
void ToLLVMVisitor::Visit(const PointerType *pointer_type) const noexcept {
  assert(pointer_type != nullptr);
  result = env.instruction_builder->getPtrTy();
}

/*
  a Slice is a tuple of (size, offset, pointer)
*/
void ToLLVMVisitor::Visit(const SliceType *slice_type) const noexcept {
  assert(slice_type != nullptr);
  auto *integer_type = Compute(env.type_interner.GetIntType(), this);
  auto *pointer_type = env.instruction_builder->getPtrTy();
  result             = llvm::StructType::get(*env.context,
                                             {integer_type, integer_type, pointer_type});
}

void ToLLVMVisitor::Visit(const TupleType *tuple_type) const noexcept {
  assert(tuple_type != nullptr);
  std::vector<llvm::Type *> llvm_element_types(
      tuple_type->GetElements().size());
  auto transform_element = [&](Type::Pointer type) {
    return Compute(type, this);
  };
  std::transform(tuple_type->begin(),
                 tuple_type->end(),
                 llvm_element_types.begin(),
                 transform_element);
  result = llvm::StructType::get(*env.context, llvm_element_types);
}

void ToLLVMVisitor::Visit(const VoidType *void_type) const noexcept {
  assert(void_type != nullptr);
  result = env.instruction_builder->getVoidTy();
}

[[nodiscard]] auto ToLLVM(Type::Pointer type, Environment &env) noexcept
    -> llvm::Type * {
  ToLLVMVisitor visitor(env);
  return visitor.Compute(type, &visitor);
}
} // namespace pink
