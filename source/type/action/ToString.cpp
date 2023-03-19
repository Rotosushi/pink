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
class TypeToStringVisitor
    : public ConstVisitorResult<TypeToStringVisitor, Type::Pointer, char>,
      public ConstTypeVisitor {
  // NOLINTNEXTLINE
  std::string &buffer;

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

  TypeToStringVisitor(std::string &buffer) noexcept
      : ConstVisitorResult{},
        ConstTypeVisitor{},
        buffer{buffer} {}
};

void TypeToStringVisitor::Visit(const ArrayType *array_type) const noexcept {
  buffer.append("[");
  Compute(array_type->GetElementType(), this);
  buffer.append("; ");
  buffer.append(std::to_string(array_type->GetSize()));
  buffer.append("]");
}

void TypeToStringVisitor::Visit(
    const BooleanType *boolean_type) const noexcept {
  assert(boolean_type != nullptr);
  buffer.append("Boolean");
}

void TypeToStringVisitor::Visit(
    const CharacterType *character_type) const noexcept {
  assert(character_type != nullptr);
  buffer.append("Character");
}

void TypeToStringVisitor::Visit(
    const FunctionType *function_type) const noexcept {
  assert(function_type != nullptr);
  buffer.append("(");

  std::size_t index  = 0;
  std::size_t length = function_type->GetArguments().size();
  for (const auto *type : function_type->GetArguments()) {
    Compute(type, this);

    if (index < (length - 1)) {
      buffer.append(", ");
    }
    index++;
  }
  buffer.append(") -> ");
  Compute(function_type->GetReturnType(), this);
}

void TypeToStringVisitor::Visit(
    const TypeVariable *identifier_type) const noexcept {
  assert(identifier_type != nullptr);
  buffer.append(identifier_type->Identifier());
}

void TypeToStringVisitor::Visit(
    const IntegerType *integer_type) const noexcept {
  assert(integer_type != nullptr);
  buffer.append("Integer");
}

void TypeToStringVisitor::Visit(const NilType *nil_type) const noexcept {
  assert(nil_type != nullptr);
  buffer.append("Nil");
}

void TypeToStringVisitor::Visit(
    const PointerType *pointer_type) const noexcept {
  assert(pointer_type != nullptr);
  buffer.append("Pointer<");
  Compute(pointer_type->GetPointeeType(), this);
  buffer.append(">");
}

void TypeToStringVisitor::Visit(const SliceType *slice_type) const noexcept {
  assert(slice_type != nullptr);
  buffer.append("Slice<");
  Compute(slice_type->GetPointeeType(), this);
  buffer.append(">");
}

void TypeToStringVisitor::Visit(const TupleType *tuple_type) const noexcept {
  assert(tuple_type != nullptr);
  buffer.append("(");

  std::size_t index  = 0;
  std::size_t length = tuple_type->GetElements().size();
  for (const auto *type : *tuple_type) {
    Compute(type, this);

    if (index < (length - 1)) {
      buffer.append(", ");
    }
    index++;
  }
  buffer.append(")");
}

void TypeToStringVisitor::Visit(const VoidType *void_type) const noexcept {
  assert(void_type != nullptr);
  buffer.append("Void");
}

[[nodiscard]] auto ToString(Type::Pointer type) noexcept -> std::string {
  assert(type != nullptr);
  std::string         buffer;
  TypeToStringVisitor visitor{buffer};
  visitor.Compute(type, &visitor);
  return buffer;
}
} // namespace pink
