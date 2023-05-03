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

#include "catch2/catch_test_macros.hpp"

#include "aux/StringInterner.h"
#include "type/interner/TypeInterner.h"

TEST_CASE("aux/TypeInterner", "[unit][aux]") {
  pink::Type::Annotations annotations;
  pink::StringInterner    string_interner;
  pink::TypeInterner      interner;

  // NilType equality
  pink::Type::Pointer nil_type = interner.GetNilType(annotations);
  REQUIRE(nil_type != nullptr);

  pink::Type::Pointer another_nil_type = interner.GetNilType(annotations);
  REQUIRE(another_nil_type != nullptr);
  REQUIRE(nil_type == another_nil_type);

  // BooleanType equality
  pink::Type::Pointer boolean_type = interner.GetBoolType(annotations);
  REQUIRE(boolean_type != nullptr);
  REQUIRE(boolean_type != nil_type);

  pink::Type::Pointer another_boolean_type = interner.GetBoolType(annotations);
  REQUIRE(another_boolean_type != nullptr);
  REQUIRE(boolean_type == another_boolean_type);

  // IntegerType equality
  pink::Type::Pointer integer_type = interner.GetIntType(annotations);
  REQUIRE(integer_type != nullptr);
  REQUIRE(integer_type != nil_type);

  pink::Type::Pointer another_integer_type = interner.GetIntType(annotations);
  REQUIRE(another_integer_type != nullptr);
  REQUIRE(integer_type == another_integer_type);

  // CharacterType Equality
  pink::Type::Pointer character_type = interner.GetCharacterType(annotations);
  REQUIRE(character_type != nullptr);
  REQUIRE(character_type != nil_type);

  pink::Type::Pointer another_character_type =
      interner.GetCharacterType(annotations);
  REQUIRE(another_character_type != nullptr);
  REQUIRE(character_type == another_character_type);

  // VoidType Equality
  pink::Type::Pointer void_type = interner.GetVoidType(annotations);
  REQUIRE(void_type != nullptr);
  REQUIRE(void_type != nil_type);

  pink::Type::Pointer another_void_type = interner.GetVoidType(annotations);
  REQUIRE(another_void_type != nullptr);
  REQUIRE(void_type == another_void_type);

  // PointerType Equality
  pink::Type::Pointer integer_pointer_type =
      interner.GetPointerType(annotations, integer_type);
  REQUIRE(integer_pointer_type != nullptr);
  REQUIRE(integer_pointer_type != integer_type);

  pink::Type::Pointer another_integer_pointer_type =
      interner.GetPointerType(annotations, integer_type);
  REQUIRE(another_integer_pointer_type != nullptr);
  REQUIRE(integer_pointer_type == another_integer_pointer_type);

  pink::Type::Pointer boolean_pointer_type =
      interner.GetPointerType(annotations, boolean_type);
  REQUIRE(boolean_pointer_type != nullptr);
  REQUIRE(boolean_pointer_type != integer_pointer_type);

  // SliceType equality
  pink::Type::Pointer integer_slice_type =
      interner.GetSliceType(annotations, integer_type);
  REQUIRE(integer_slice_type != nullptr);
  REQUIRE(integer_slice_type != integer_type);

  pink::Type::Pointer another_integer_slice_type =
      interner.GetSliceType(annotations, integer_type);
  REQUIRE(another_integer_slice_type != nullptr);
  REQUIRE(integer_slice_type == another_integer_slice_type);

  pink::Type::Pointer boolean_slice_type =
      interner.GetSliceType(annotations, boolean_type);
  REQUIRE(boolean_slice_type != nullptr);
  REQUIRE(boolean_slice_type != integer_slice_type);

  // ArrayType equality
  const std::size_t   five = 5;
  const std::size_t   ten  = 10;
  pink::Type::Pointer five_integer_array_type =
      interner.GetArrayType(annotations, five, integer_type);
  REQUIRE(five_integer_array_type != nullptr);
  REQUIRE(five_integer_array_type != integer_type);

  pink::Type::Pointer another_five_integer_array_type =
      interner.GetArrayType(annotations, five, integer_type);
  REQUIRE(another_five_integer_array_type != nullptr);
  REQUIRE(another_five_integer_array_type == five_integer_array_type);

  pink::Type::Pointer ten_integer_array_type =
      interner.GetArrayType(annotations, ten, integer_type);
  REQUIRE(ten_integer_array_type != nullptr);
  REQUIRE(ten_integer_array_type != five_integer_array_type);

  pink::Type::Pointer five_boolean_array_type =
      interner.GetArrayType(annotations, five, boolean_type);
  REQUIRE(five_boolean_array_type != nullptr);
  REQUIRE(five_boolean_array_type != five_integer_array_type);

  // TupleType Equality
  pink::Type::Pointer integer_pair_type =
      interner.GetTupleType(annotations, {integer_type, integer_type});
  REQUIRE(integer_pair_type != nullptr);
  REQUIRE(integer_pair_type != integer_type);

  pink::Type::Pointer integer_triple_type =
      interner.GetTupleType(annotations,
                            {integer_type, integer_type, integer_type});
  REQUIRE(integer_triple_type != nullptr);
  REQUIRE(integer_triple_type != integer_pair_type);

  pink::Type::Pointer boolean_pair_type =
      interner.GetTupleType(annotations, {boolean_type, boolean_type});
  REQUIRE(boolean_pair_type != nullptr);
  REQUIRE(boolean_pair_type != integer_pair_type);

  pink::Type::Pointer int_bool_pair =
      interner.GetTupleType(annotations, {integer_type, boolean_type});
  REQUIRE(int_bool_pair != nullptr);
  REQUIRE(int_bool_pair != integer_pair_type);
  REQUIRE(int_bool_pair != boolean_pair_type);

  // FunctionType equality
  pink::Type::Pointer integer_binop_function =
      interner.GetFunctionType(annotations,
                               integer_type,
                               {integer_type, integer_type});
  REQUIRE(integer_binop_function != nullptr);
  REQUIRE(integer_binop_function != integer_type);

  pink::Type::Pointer boolean_binop_function =
      interner.GetFunctionType(annotations,
                               boolean_type,
                               {boolean_type, boolean_type});
  REQUIRE(boolean_binop_function != nullptr);
  REQUIRE(boolean_binop_function != integer_binop_function);
  REQUIRE(boolean_binop_function != integer_binop_function);

  pink::Type::Pointer integer_unop_function =
      interner.GetFunctionType(annotations, integer_type, {integer_type});
  REQUIRE(integer_unop_function != nullptr);
  REQUIRE(integer_unop_function != integer_binop_function);

  pink::Type::Pointer boolean_unop_function =
      interner.GetFunctionType(annotations, boolean_type, {boolean_type});
  REQUIRE(boolean_unop_function != nullptr);
  REQUIRE(boolean_unop_function != boolean_binop_function);
  REQUIRE(integer_unop_function != boolean_unop_function);

  pink::InternedString variable_T = string_interner.Intern("T");
  pink::InternedString variable_U = string_interner.Intern("U");
  pink::Type::Pointer  type_variable_T =
      interner.GetTypeVariable(annotations, variable_T);
  pink::Type::Pointer type_variable_U =
      interner.GetTypeVariable(annotations, variable_U);
  pink::Type::Pointer duplicate_U =
      interner.GetTypeVariable(annotations, variable_U);
  REQUIRE(type_variable_T != nullptr);
  REQUIRE(type_variable_U != nullptr);
  REQUIRE(type_variable_U == duplicate_U);
  REQUIRE(type_variable_T != duplicate_U);
}