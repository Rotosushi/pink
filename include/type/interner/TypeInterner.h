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

/**
 * @file TypeInterner.h
 * @brief Header for class TypeInterner
 * @version 0.1
 *
 */
#pragma once
#include <memory>
#include <string>
#include <vector>

#include "type/All.h"

namespace pink {
/**
 * @brief Interns Types, so that only one instance of each unique type is
 * constructed
 */
class TypeInterner {
private:
  NilType       nil_type;
  BooleanType   bool_type;
  IntegerType   int_type;
  CharacterType character_type;
  VoidType      void_type;

  // #TODO: vector may not be the best solution as
  // we scale up to larger programs with an abundance of types.
  // a map might help access time scale favorably with size.
  // the only question is how? what is the key and what is the
  // value? the 'key' is truly the uniqueness of each type.
  std::vector<std::unique_ptr<FunctionType>> function_types;
  std::vector<std::unique_ptr<PointerType>>  pointer_types;
  std::vector<std::unique_ptr<ArrayType>>    array_types;
  std::vector<std::unique_ptr<SliceType>>    slice_types;
  std::vector<std::unique_ptr<TupleType>>    tuple_types;
  std::vector<std::unique_ptr<TypeVariable>> identifier_types;

public:
  TypeInterner() noexcept
      : nil_type(this),
        bool_type(this),
        int_type(this),
        character_type(this),
        void_type(this) {}
  ~TypeInterner()                                             = default;
  TypeInterner(const TypeInterner &other)                     = delete;
  TypeInterner(TypeInterner &&other)                          = default;
  auto operator=(const TypeInterner &other) -> TypeInterner & = delete;
  auto operator=(TypeInterner &&other) -> TypeInterner      & = default;

  auto GetNilType() -> NilType::Pointer;
  auto GetBoolType() -> BooleanType::Pointer;
  auto GetIntType() -> IntegerType::Pointer;
  auto GetCharacterType() -> CharacterType::Pointer;
  auto GetVoidType() -> VoidType::Pointer;

  auto GetFunctionType(Type::Pointer             ret_type,
                       FunctionType::Arguments &&arg_types)
      -> FunctionType::Pointer;

  auto GetPointerType(Type::Pointer pointee_type) -> PointerType::Pointer;
  auto GetSliceType(Type::Pointer pointee_type) -> SliceType::Pointer;

  auto GetArrayType(std::size_t size, Type::Pointer element_type)
      -> ArrayType::Pointer;

  auto GetTupleType(TupleType::Elements &&elements) -> TupleType::Pointer;

  auto GetTextType(std::size_t length) -> ArrayType::Pointer;

  auto GetTypeVariable(InternedString identifier) -> TypeVariable::Pointer;
};

} // namespace pink

// ---
