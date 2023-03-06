#include "type/action/Substitution.h"
#include "type/All.h"
#include "type/visitor/TypeVisitor.h"

#include "visitor/VisitorResult.h"

#include "aux/Environment.h"

namespace pink {
class SubstitutionVisitor : public ConstVisitorResult<SubstitutionVisitor,
                                                      Type::Pointer,
                                                      Type::Pointer>,
                            public ConstTypeVisitor {
private:
  Type::Pointer source_type;
  Type::Pointer type_variable;

public:
  SubstitutionVisitor(Type::Pointer source_type, Type::Pointer type_variable)
      : ConstVisitorResult(),
        source_type(source_type),
        type_variable(type_variable) {}

  void Visit(const ArrayType *array_type) const noexcept override;
  void Visit(const BooleanType *boolean_type) const noexcept override;
  void Visit(const CharacterType *character_type) const noexcept override;
  void Visit(const FunctionType *function_type) const noexcept override;
  void Visit(const IdentifierType *identifier_type) const noexcept override;
  void Visit(const IntegerType *integer_type) const noexcept override;
  void Visit(const NilType *nil_type) const noexcept override;
  void Visit(const PointerType *pointer_type) const noexcept override;
  void Visit(const SliceType *slice_type) const noexcept override;
  void Visit(const TupleType *tuple_type) const noexcept override;
  void Visit(const VoidType *void_type) const noexcept override;
};

void SubstitutionVisitor::Visit(const ArrayType *array_type) const noexcept {
  auto *context = array_type->GetContext();
  result        = context->GetArrayType(array_type->GetSize(),
                                 Compute(array_type->GetElementType(), this));
}
void SubstitutionVisitor::Visit(
    const BooleanType *boolean_type) const noexcept {
  result = boolean_type;
}
void SubstitutionVisitor::Visit(
    const CharacterType *character_type) const noexcept {
  result = character_type;
}
void SubstitutionVisitor::Visit(
    const FunctionType *function_type) const noexcept {
  auto                      *context = function_type->GetContext();
  std::vector<Type::Pointer> arg_types;
  arg_types.reserve(function_type->GetArguments().size());
  for (const auto *arg_type : function_type->GetArguments()) {
    arg_types.emplace_back(Compute(arg_type, this));
  }

  result =
      context->GetFunctionType(Compute(function_type->GetReturnType(), this),
                               std::move(arg_types));
}

void SubstitutionVisitor::Visit(
    const IdentifierType *identifier_type) const noexcept {
  if (identifier_type == type_variable) {
    result = source_type;
  } else {
    result = identifier_type;
  }
}

void SubstitutionVisitor::Visit(
    const IntegerType *integer_type) const noexcept {
  result = integer_type;
}

void SubstitutionVisitor::Visit(const NilType *nil_type) const noexcept {
  result = nil_type;
}

void SubstitutionVisitor::Visit(
    const PointerType *pointer_type) const noexcept {
  auto *context = pointer_type->GetContext();
  result =
      context->GetPointerType(Compute(pointer_type->GetPointeeType(), this));
}

void SubstitutionVisitor::Visit(const SliceType *slice_type) const noexcept {
  auto *context = slice_type->GetContext();
  result = context->GetSliceType(Compute(slice_type->GetPointeeType(), this));
}

void SubstitutionVisitor::Visit(const TupleType *tuple_type) const noexcept {
  auto                      *context = tuple_type->GetContext();
  std::vector<Type::Pointer> element_types;
  element_types.reserve(tuple_type->GetElements().size());
  for (const auto *element_type : tuple_type->GetElements()) {
    element_types.emplace_back(Compute(element_type, this));
  }
  result = context->GetTupleType(std::move(element_types));
}

void SubstitutionVisitor::Visit(const VoidType *void_type) const noexcept {
  result = void_type;
}

auto Substitution(Type::Pointer type_variable,
                  Type::Pointer source_type,
                  Type::Pointer target_type) noexcept -> Type::Pointer {
  SubstitutionVisitor visitor(source_type, type_variable);
  return visitor.Compute(target_type, &visitor);
}
} // namespace pink
