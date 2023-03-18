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

#include "type/action/ToString.h"
#include "type/visitor/TypeVisitor.h"
#include "visitor/VisitorResult.h"

#include "type/All.h"

namespace pink {
class TypeToStringVisitor : public ConstVisitorResult<TypeToStringVisitor,
                                                      Type::Pointer,
                                                      std::string>,
                            public ConstTypeVisitor {
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
};

void TypeToStringVisitor::Visit(const ArrayType *array_type) const noexcept {
  result = "[";
  result += Compute(array_type->GetElementType(), this);
  result += "; ";
  result += std::to_string(array_type->GetSize());
  result += "]";
}

void TypeToStringVisitor::Visit(
    const BooleanType *boolean_type) const noexcept {
  assert(boolean_type != nullptr);
  result = "Boolean";
}

void TypeToStringVisitor::Visit(
    const CharacterType *character_type) const noexcept {
  assert(character_type != nullptr);
  result = "Character";
}

void TypeToStringVisitor::Visit(
    const FunctionType *function_type) const noexcept {
  assert(function_type != nullptr);
  result = "(";

  std::size_t index  = 0;
  std::size_t length = function_type->GetArguments().size();
  for (const auto *type : function_type->GetArguments()) {
    result += Compute(type, this);

    if (index < (length - 1)) {
      result += ", ";
    }
    index++;
  }
  result += ") -> ";
  result += Compute(function_type->GetReturnType(), this);
}

void TypeToStringVisitor::Visit(
    const TypeVariable *identifier_type) const noexcept {
  assert(identifier_type != nullptr);
  result = identifier_type->Identifier();
}

void TypeToStringVisitor::Visit(
    const IntegerType *integer_type) const noexcept {
  assert(integer_type != nullptr);
  result = "Integer";
}

void TypeToStringVisitor::Visit(const NilType *nil_type) const noexcept {
  assert(nil_type != nullptr);
  result = "Nil";
}

void TypeToStringVisitor::Visit(
    const PointerType *pointer_type) const noexcept {
  assert(pointer_type != nullptr);
  result = "Pointer<";
  result += Compute(pointer_type->GetPointeeType(), this);
  result += ">";
}

void TypeToStringVisitor::Visit(const SliceType *slice_type) const noexcept {
  assert(slice_type != nullptr);
  result = "Slice<";
  result += Compute(slice_type->GetPointeeType(), this);
  result += ">";
}

void TypeToStringVisitor::Visit(const TupleType *tuple_type) const noexcept {
  assert(tuple_type != nullptr);
  result = "(";

  std::size_t index  = 0;
  std::size_t length = tuple_type->GetElements().size();
  for (const auto *type : *tuple_type) {
    result += Compute(type, this);

    if (index < (length - 1)) {
      result += ", ";
    }
    index++;
  }
  result += ")";
}

void TypeToStringVisitor::Visit(const VoidType *void_type) const noexcept {
  assert(void_type != nullptr);
  result = "Void";
}

[[nodiscard]] auto ToString(Type::Pointer type) noexcept -> std::string {
  assert(type != nullptr);
  TypeToStringVisitor visitor;
  return visitor.Compute(type, &visitor);
}
} // namespace pink
