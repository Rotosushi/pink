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

#pragma once

#include "visitor/VisitorResult.h"

namespace pink {
class ArrayType;
class BooleanType;
class CharacterType;
class FunctionType;
class TypeVariable;
class IntegerType;
class NilType;
class PointerType;
class SliceType;
class TupleType;
class VoidType;

class TypeVisitor {
public:
  virtual void Visit(ArrayType *array_type) noexcept         = 0;
  virtual void Visit(BooleanType *boolean_type) noexcept     = 0;
  virtual void Visit(CharacterType *character_type) noexcept = 0;
  virtual void Visit(FunctionType *function_type) noexcept   = 0;
  virtual void Visit(TypeVariable *identifier_type) noexcept = 0;
  virtual void Visit(IntegerType *integer_type) noexcept     = 0;
  virtual void Visit(NilType *nil_type) noexcept             = 0;
  virtual void Visit(PointerType *pointer_type) noexcept     = 0;
  virtual void Visit(SliceType *slice_type) noexcept         = 0;
  virtual void Visit(TupleType *tuple_type) noexcept         = 0;
  virtual void Visit(VoidType *void_type) noexcept           = 0;

  TypeVisitor() noexcept                                             = default;
  virtual ~TypeVisitor() noexcept                                    = default;
  TypeVisitor(const TypeVisitor &other) noexcept                     = default;
  TypeVisitor(TypeVisitor &&other) noexcept                          = default;
  auto operator=(const TypeVisitor &other) noexcept -> TypeVisitor & = default;
  auto operator=(TypeVisitor &&other) noexcept -> TypeVisitor      & = default;
};

class ConstTypeVisitor {
public:
  virtual void Visit(const ArrayType *array_type) const noexcept         = 0;
  virtual void Visit(const BooleanType *boolean_type) const noexcept     = 0;
  virtual void Visit(const CharacterType *character_type) const noexcept = 0;
  virtual void Visit(const FunctionType *function_type) const noexcept   = 0;
  virtual void Visit(const TypeVariable *identifier_type) const noexcept = 0;
  virtual void Visit(const IntegerType *integer_type) const noexcept     = 0;
  virtual void Visit(const NilType *nil_type) const noexcept             = 0;
  virtual void Visit(const PointerType *pointer_type) const noexcept     = 0;
  virtual void Visit(const SliceType *slice_type) const noexcept         = 0;
  virtual void Visit(const TupleType *tuple_type) const noexcept         = 0;
  virtual void Visit(const VoidType *void_type) const noexcept           = 0;

  ConstTypeVisitor() noexcept                              = default;
  virtual ~ConstTypeVisitor() noexcept                     = default;
  ConstTypeVisitor(const ConstTypeVisitor &other) noexcept = default;
  ConstTypeVisitor(ConstTypeVisitor &&other) noexcept      = default;
  auto operator=(const ConstTypeVisitor &other) noexcept
      -> ConstTypeVisitor & = default;
  auto operator=(ConstTypeVisitor &&other) noexcept
      -> ConstTypeVisitor & = default;
};
} // namespace pink
