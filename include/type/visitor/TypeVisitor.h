#pragma once

#include "visitor/VisitorResult.h"

namespace pink {
class ArrayType;
class BooleanType;
class CharacterType;
class FunctionType;
class IdentifierType;
class IntegerType;
class NilType;
class PointerType;
class SliceType;
class TupleType;
class VoidType;

class TypeVisitor {
public:
  virtual void Visit(ArrayType *array_type) noexcept           = 0;
  virtual void Visit(BooleanType *boolean_type) noexcept       = 0;
  virtual void Visit(CharacterType *character_type) noexcept   = 0;
  virtual void Visit(FunctionType *function_type) noexcept     = 0;
  virtual void Visit(IdentifierType *identifier_type) noexcept = 0;
  virtual void Visit(IntegerType *integer_type) noexcept       = 0;
  virtual void Visit(NilType *nil_type) noexcept               = 0;
  virtual void Visit(PointerType *pointer_type) noexcept       = 0;
  virtual void Visit(SliceType *slice_type) noexcept           = 0;
  virtual void Visit(TupleType *tuple_type) noexcept           = 0;
  virtual void Visit(VoidType *void_type) noexcept             = 0;

  TypeVisitor() noexcept                                             = default;
  virtual ~TypeVisitor() noexcept                                    = default;
  TypeVisitor(const TypeVisitor &other) noexcept                     = default;
  TypeVisitor(TypeVisitor &&other) noexcept                          = default;
  auto operator=(const TypeVisitor &other) noexcept -> TypeVisitor & = default;
  auto operator=(TypeVisitor &&other) noexcept -> TypeVisitor      & = default;
};

class ConstTypeVisitor {
public:
  virtual void Visit(const ArrayType *array_type) const noexcept           = 0;
  virtual void Visit(const BooleanType *boolean_type) const noexcept       = 0;
  virtual void Visit(const CharacterType *character_type) const noexcept   = 0;
  virtual void Visit(const FunctionType *function_type) const noexcept     = 0;
  virtual void Visit(const IdentifierType *identifier_type) const noexcept = 0;
  virtual void Visit(const IntegerType *integer_type) const noexcept       = 0;
  virtual void Visit(const NilType *nil_type) const noexcept               = 0;
  virtual void Visit(const PointerType *pointer_type) const noexcept       = 0;
  virtual void Visit(const SliceType *slice_type) const noexcept           = 0;
  virtual void Visit(const TupleType *tuple_type) const noexcept           = 0;
  virtual void Visit(const VoidType *void_type) const noexcept             = 0;

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
