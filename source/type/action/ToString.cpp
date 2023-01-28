#include "type/action/ToString.h"

#include "type/All.h"

namespace pink {
class ToStringVisitor
    : public ConstVisitorResult<ToStringVisitor, const Type::Pointer,
                                std::string>,
      public ConstTypeVisitor {
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
};

void ToStringVisitor::Visit(const ArrayType *array_type) const noexcept {
  result = "[";
  result += Compute(array_type->GetElementType(), this);
  result += "; ";
  result += std::to_string(array_type->GetSize());
  result += "]";
}

void ToStringVisitor::Visit(const BooleanType *boolean_type) const noexcept {
  result = "Boolean";
}

void ToStringVisitor::Visit(
    const CharacterType *character_type) const noexcept {
  result = "Character";
}

void ToStringVisitor::Visit(const FunctionType *function_type) const noexcept {
  result             = "(";

  std::size_t index  = 0;
  std::size_t length = function_type->GetArguments().size();
  for (auto *type : function_type->GetArguments()) {
    result += Compute(type, this);

    if (index < (length - 1)) {
      result += ", ";
    }
    index++;
  }
  result += ") -> ";
  result += Compute(function_type->GetReturnType(), this);
}

void ToStringVisitor::Visit(const IntegerType *integer_type) const noexcept {
  result = "Integer";
}

void ToStringVisitor::Visit(const NilType *nil_type) const noexcept {
  result = "Nil";
}

void ToStringVisitor::Visit(const PointerType *pointer_type) const noexcept {
  result = "Pointer<";
  result += Compute(pointer_type->GetPointeeType(), this);
  result += ">";
}

void ToStringVisitor::Visit(const SliceType *slice_type) const noexcept {
  result = "Slice<";
  result += Compute(slice_type->GetPointeeType(), this);
  result += ">";
}

void ToStringVisitor::Visit(const TupleType *tuple_type) const noexcept {
  result             = "(";

  std::size_t index  = 0;
  std::size_t length = tuple_type->GetElements().size();
  for (auto *type : *tuple_type) {
    result += Compute(type, this);

    if (index < (length - 1)) {
      result += ", ";
    }
    index++;
  }
  result += ")";
}

void ToStringVisitor::Visit(const VoidType *void_type) const noexcept {
  result = "Void";
}

[[nodiscard]] auto ToString(const Type::Pointer type) noexcept -> std::string {
  ToStringVisitor visitor;
  return visitor.Compute(type, &visitor);
}
} // namespace pink
