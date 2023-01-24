#include "type/action/ToString.h"

#include "type/All.h"

namespace pink {
void TypeToString::Visit(const ArrayType *array_type) noexcept {
  result = "[";
  result += Compute(array_type->GetElementType());
  result += "; ";
  result += std::to_string(array_type->GetSize());
  result += "]";
}

void TypeToString::Visit(const BooleanType *boolean_type) noexcept {
  result = "Boolean";
}

void TypeToString::Visit(const CharacterType *character_type) noexcept {
  result = "Character";
}

void TypeToString::Visit(const FunctionType *function_type) noexcept {
  result = "(";

  std::size_t index = 0;
  std::size_t length = function_type->GetArguments().size();
  for (auto *type : function_type->GetArguments()) {
    result += Compute(type);

    if (index < (length - 1)) {
      result += ", ";
    }
    index++;
  }
  result += ") -> ";
  result += Compute(function_type->GetReturnType());
}

void TypeToString::Visit(const IntegerType *integer_type) noexcept {
  result = "Integer";
}

void TypeToString::Visit(const NilType *nil_type) noexcept { result = "Nil"; }

void TypeToString::Visit(const PointerType *pointer_type) noexcept {
  result = "Pointer<";
  result += Compute(pointer_type->GetPointeeType());
  result += ">";
}

void TypeToString::Visit(const SliceType *slice_type) noexcept {
  result = "Slice<";
  result += Compute(slice_type->GetPointeeType());
  result += ">";
}

void TypeToString::Visit(const TupleType *tuple_type) noexcept {
  result = "(";

  std::size_t index = 0;
  std::size_t length = tuple_type->GetElements().size();
  for (auto *type : *tuple_type) {
    result += Compute(type);

    if (index < (length - 1)) {
      result += ", ";
    }
    index++;
  }
  result += ")";
}

void TypeToString::Visit(const VoidType *void_type) noexcept {
  result = "Void";
}

[[nodiscard]] auto ToString(const Type::Pointer type) noexcept -> std::string {
  return TypeToString::Compute(type);
}
} // namespace pink
