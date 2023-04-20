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

#include <algorithm>

#include "type/interner/TypeInterner.h"

namespace pink {

auto TypeInterner::GetNilType() -> NilType::Pointer { return &nil_type; }
auto TypeInterner::GetBoolType() -> BooleanType::Pointer { return &bool_type; }
auto TypeInterner::GetIntType() -> IntegerType::Pointer { return &int_type; }
auto TypeInterner::GetCharacterType() -> CharacterType::Pointer {
  return &character_type;
}
auto TypeInterner::GetVoidType() -> VoidType::Pointer { return &void_type; }

auto TypeInterner::GetFunctionType(Type::Pointer             return_type,
                                   FunctionType::Arguments &&arg_types)
    -> FunctionType::Pointer {
  auto possible =
      std::make_unique<FunctionType>(this, return_type, std::move(arg_types));

  for (auto &function_type : function_types) {
    if (possible->Equals(function_type.get())) {
      return function_type.get();
    }
  }

  FunctionType::Pointer result = possible.get();
  function_types.emplace_back(std::move(possible));
  return result;
}

auto TypeInterner::GetPointerType(Type::Pointer pointee_type)
    -> PointerType::Pointer {
  auto possible = std::make_unique<PointerType>(this, pointee_type);

  for (auto &pointer_type : pointer_types) {
    if (possible->Equals(pointer_type.get())) {
      return pointer_type.get();
    }
  }

  PointerType::Pointer result = possible.get();
  pointer_types.emplace_back(std::move(possible));

  return result;
}

auto TypeInterner::GetSliceType(Type::Pointer pointee_type)
    -> SliceType::Pointer {
  auto possible = std::make_unique<SliceType>(this, pointee_type);

  for (auto &slice_type : slice_types) {
    if (possible->Equals(slice_type.get())) {
      return slice_type.get();
    }
  }

  SliceType::Pointer result = possible.get();
  slice_types.emplace_back(std::move(possible));
  return result;
}

auto TypeInterner::GetArrayType(std::size_t size, Type::Pointer member_type)
    -> ArrayType::Pointer {
  auto possible = std::make_unique<ArrayType>(this, size, member_type);

  for (auto &array_type : array_types) {
    if (possible->Equals(array_type.get())) {
      return array_type.get();
    }
  }

  ArrayType *result = possible.get();
  array_types.emplace_back(std::move(possible));

  return result;
}

auto TypeInterner::GetTupleType(TupleType::Elements &&member_types)
    -> TupleType::Pointer {
  auto possible = std::make_unique<TupleType>(this, std::move(member_types));

  for (auto &tuple_type : tuple_types) {
    if (possible->Equals(tuple_type.get())) {
      return tuple_type.get();
    }
  }

  TupleType *result = possible.get();
  tuple_types.emplace_back(std::move(possible));
  return result;
}

auto TypeInterner::GetTextType(std::size_t length) -> ArrayType::Pointer {
  return GetArrayType(length + 1, GetCharacterType());
}

auto TypeInterner::GetTypeVariable(InternedString identifier)
    -> TypeVariable::Pointer {
  auto possible = std::make_unique<TypeVariable>(this, identifier);
  for (auto &identifier_type : identifier_types) {
    if (possible->Equals(identifier_type.get())) {
      return identifier_type.get();
    }
  }

  TypeVariable::Pointer result = possible.get();
  identifier_types.emplace_back(std::move(possible));
  return result;
}
} // namespace pink
