#include "type/action/StructuralEquality.h"

#include "type/visitor/TypeVisitor.h"
#include "visitor/VisitorResult.h"

#include "type/All.h"

namespace pink {
class StructuralEqualityVisitor
    : public ConstVisitorResult<StructuralEqualityVisitor,
                                const Type::Pointer,
                                bool>,
      public ConstTypeVisitor {
private:
  Type::Pointer one;

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

  StructuralEqualityVisitor(Type::Pointer one) noexcept
      : ConstVisitorResult(),
        one(one) {}
  ~StructuralEqualityVisitor() noexcept override = default;
  StructuralEqualityVisitor(const StructuralEqualityVisitor &other) noexcept =
      default;
  StructuralEqualityVisitor(StructuralEqualityVisitor &&other) noexcept =
      default;
  auto operator=(const StructuralEqualityVisitor &other) noexcept
      -> StructuralEqualityVisitor & = default;
  auto operator=(StructuralEqualityVisitor &&other) noexcept
      -> StructuralEqualityVisitor & = default;
};

void StructuralEqualityVisitor::Visit(
    const ArrayType *array_type) const noexcept {
  assert(array_type != nullptr);
  const auto *other_array = llvm::dyn_cast<const ArrayType>(one);
  if (other_array == nullptr) {
    result = false;
    return;
  }

  if (array_type->GetSize() != other_array->GetSize()) {
    result = false;
    return;
  }

  result = StructuralEquality(array_type->GetElementType(),
                              other_array->GetElementType());
}

void StructuralEqualityVisitor::Visit(
    const BooleanType *boolean_type) const noexcept {
  assert(boolean_type != nullptr);
  const auto *other_boolean = llvm::dyn_cast<const BooleanType>(one);
  result                    = (other_boolean != nullptr);
}

void StructuralEqualityVisitor::Visit(
    const CharacterType *character_type) const noexcept {
  assert(character_type != nullptr);
  const auto *other_character = llvm::dyn_cast<const CharacterType>(one);
  result                      = (other_character != nullptr);
}
void StructuralEqualityVisitor::Visit(
    const FunctionType *function_type) const noexcept {
  assert(function_type != nullptr);
  const auto *other_type = llvm::dyn_cast<const FunctionType>(one);
  if (other_type == nullptr) {
    result = false;
    return;
  }

  if (function_type->GetArguments().size() !=
      other_type->GetArguments().size()) {
    result = false;
    return;
  }

  if (!StructuralEquality(function_type->GetReturnType(),
                          other_type->GetReturnType())) {
    result = false;
    return;
  }

  const auto &function_arguments = function_type->GetArguments();
  const auto &other_arguments    = other_type->GetArguments();

  for (size_t index = 0; index < function_arguments.size(); index += 1) {
    if (!StructuralEquality(function_arguments[index],
                            other_arguments[index])) {
      result = false;
      return;
    }
  }

  result = true;
}

void StructuralEqualityVisitor::Visit(
    const TypeVariable *identifier_type) const noexcept {
  assert(identifier_type != nullptr);
  const auto *other_identifier = llvm::dyn_cast<const TypeVariable>(one);
  if (other_identifier == nullptr) {
    result = false;
    return;
  }

  result = identifier_type->Identifier() == other_identifier->Identifier();
}

void StructuralEqualityVisitor::Visit(
    const IntegerType *integer_type) const noexcept {
  assert(integer_type != nullptr);
  const auto *other_integer = llvm::dyn_cast<const IntegerType>(one);
  result                    = (other_integer != nullptr);
}

void StructuralEqualityVisitor::Visit(const NilType *nil_type) const noexcept {
  assert(nil_type != nullptr);
  const auto *other_type = llvm::dyn_cast<const NilType>(one);
  result                 = (other_type != nullptr);
}
void StructuralEqualityVisitor::Visit(
    const PointerType *pointer_type) const noexcept {
  assert(pointer_type != nullptr);
  const auto *other_type = llvm::dyn_cast<const PointerType>(one);
  if (other_type == nullptr) {
    result = false;
    return;
  }

  result = StructuralEquality(pointer_type->GetPointeeType(),
                              other_type->GetPointeeType());
}

void StructuralEqualityVisitor::Visit(
    const SliceType *slice_type) const noexcept {
  assert(slice_type != nullptr);
  const auto *other_type = llvm::dyn_cast<const SliceType>(one);
  if (other_type == nullptr) {
    result = false;
    return;
  }

  result = StructuralEquality(slice_type->GetPointeeType(),
                              other_type->GetPointeeType());
}

void StructuralEqualityVisitor::Visit(
    const TupleType *tuple_type) const noexcept {
  assert(tuple_type != nullptr);
  const auto *other_type = llvm::dyn_cast<const TupleType>(one);
  if (other_type == nullptr) {
    result = false;
    return;
  }

  if (tuple_type->GetElements().size() != other_type->GetElements().size()) {
    result = false;
    return;
  }

  const auto &element_types  = tuple_type->GetElements();
  const auto &other_elements = other_type->GetElements();
  for (size_t index = 0; index < element_types.size(); index += 1) {
    if (!StructuralEquality(element_types[index], other_elements[index])) {
      result = false;
      return;
    }
  }

  result = true;
}

void StructuralEqualityVisitor::Visit(
    const VoidType *void_type) const noexcept {
  assert(void_type != nullptr);
  const auto *other_type = llvm::dyn_cast<const VoidType>(one);
  result                 = (other_type != nullptr);
}

[[nodiscard]] auto StructuralEquality(Type::Pointer one,
                                      Type::Pointer two) noexcept -> bool {
  StructuralEqualityVisitor visitor(one);
  return visitor.Compute(two, &visitor);
}

} // namespace pink
