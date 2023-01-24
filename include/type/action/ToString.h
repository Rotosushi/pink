#pragma once
#include "type/visitor/TypeVisitor.h"
#include "visitor/VisitorResult.h"

#include "type/Type.h"

namespace pink {
class TypeToString
    : public ConstVisitorResult<TypeToString, const Type::Pointer, std::string>,
      public ConstTypeVisitor {
public:
  void Visit(const ArrayType *array_type) noexcept override;
  void Visit(const BooleanType *boolean_type) noexcept override;
  void Visit(const CharacterType *character_type) noexcept override;
  void Visit(const FunctionType *function_type) noexcept override;
  void Visit(const IntegerType *integer_type) noexcept override;
  void Visit(const NilType *nil_type) noexcept override;
  void Visit(const PointerType *pointer_type) noexcept override;
  void Visit(const SliceType *slice_type) noexcept override;
  void Visit(const TupleType *tuple_type) noexcept override;
  void Visit(const VoidType *void_type) noexcept override;
};

/**
 * @brief Computes the string representation of the given Type
 *
 * @param type
 * @return std::string
 */
[[nodiscard]] auto ToString(const Type::Pointer type) noexcept -> std::string;
} // namespace pink
