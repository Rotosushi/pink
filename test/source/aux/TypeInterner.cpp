#include "catch2/catch_test_macros.hpp"

#include "type/interner/TypeInterner.h"

TEST_CASE("aux/TypeInterner", "[unit][aux]") {
  pink::TypeInterner interner;

  // NilType equality
  pink::Type::Pointer nil_type = interner.GetNilType();
  REQUIRE(nil_type != nullptr);

  pink::Type::Pointer another_nil_type = interner.GetNilType();
  REQUIRE(another_nil_type != nullptr);
  REQUIRE(nil_type == another_nil_type);

  // BooleanType equality
  pink::Type::Pointer boolean_type = interner.GetBoolType();
  REQUIRE(boolean_type != nullptr);
  REQUIRE(boolean_type != nil_type);

  pink::Type::Pointer another_boolean_type = interner.GetBoolType();
  REQUIRE(another_boolean_type != nullptr);
  REQUIRE(boolean_type == another_boolean_type);

  // IntegerType equality
  pink::Type::Pointer integer_type = interner.GetIntType();
  REQUIRE(integer_type != nullptr);
  REQUIRE(integer_type != nil_type);

  pink::Type::Pointer another_integer_type = interner.GetIntType();
  REQUIRE(another_integer_type != nullptr);
  REQUIRE(integer_type == another_integer_type);

  // CharacterType Equality
  pink::Type::Pointer character_type = interner.GetCharacterType();
  REQUIRE(character_type != nullptr);
  REQUIRE(character_type != nil_type);

  pink::Type::Pointer another_character_type = interner.GetCharacterType();
  REQUIRE(another_character_type != nullptr);
  REQUIRE(character_type == another_character_type);

  // VoidType Equality
  pink::Type::Pointer void_type = interner.GetVoidType();
  REQUIRE(void_type != nullptr);
  REQUIRE(void_type != nil_type);

  pink::Type::Pointer another_void_type = interner.GetVoidType();
  REQUIRE(another_void_type != nullptr);
  REQUIRE(void_type == another_void_type);

  // PointerType Equality
  pink::Type::Pointer integer_pointer_type =
      interner.GetPointerType(integer_type);
  REQUIRE(integer_pointer_type != nullptr);
  REQUIRE(integer_pointer_type != integer_type);

  pink::Type::Pointer another_integer_pointer_type =
      interner.GetPointerType(integer_type);
  REQUIRE(another_integer_pointer_type != nullptr);
  REQUIRE(integer_pointer_type == another_integer_pointer_type);

  pink::Type::Pointer boolean_pointer_type =
      interner.GetPointerType(boolean_type);
  REQUIRE(boolean_pointer_type != nullptr);
  REQUIRE(boolean_pointer_type != integer_pointer_type);

  // SliceType equality
  pink::Type::Pointer integer_slice_type = interner.GetSliceType(integer_type);
  REQUIRE(integer_slice_type != nullptr);
  REQUIRE(integer_slice_type != integer_type);

  pink::Type::Pointer another_integer_slice_type =
      interner.GetSliceType(integer_type);
  REQUIRE(another_integer_slice_type != nullptr);
  REQUIRE(integer_slice_type == another_integer_slice_type);

  pink::Type::Pointer boolean_slice_type = interner.GetSliceType(boolean_type);
  REQUIRE(boolean_slice_type != nullptr);
  REQUIRE(boolean_slice_type != integer_slice_type);

  // ArrayType equality
  const std::size_t   five = 5;
  const std::size_t   ten  = 10;
  pink::Type::Pointer five_integer_array_type =
      interner.GetArrayType(five, integer_type);
  REQUIRE(five_integer_array_type != nullptr);
  REQUIRE(five_integer_array_type != integer_type);

  pink::Type::Pointer another_five_integer_array_type =
      interner.GetArrayType(five, integer_type);
  REQUIRE(another_five_integer_array_type != nullptr);
  REQUIRE(another_five_integer_array_type == five_integer_array_type);

  pink::Type::Pointer ten_integer_array_type =
      interner.GetArrayType(ten, integer_type);
  REQUIRE(ten_integer_array_type != nullptr);
  REQUIRE(ten_integer_array_type != five_integer_array_type);

  pink::Type::Pointer five_boolean_array_type =
      interner.GetArrayType(five, boolean_type);
  REQUIRE(five_boolean_array_type != nullptr);
  REQUIRE(five_boolean_array_type != five_integer_array_type);

  // TupleType Equality
  pink::Type::Pointer integer_pair_type =
      interner.GetTupleType({integer_type, integer_type});
  REQUIRE(integer_pair_type != nullptr);
  REQUIRE(integer_pair_type != integer_type);

  pink::Type::Pointer integer_triple_type =
      interner.GetTupleType({integer_type, integer_type, integer_type});
  REQUIRE(integer_triple_type != nullptr);
  REQUIRE(integer_triple_type != integer_pair_type);

  pink::Type::Pointer boolean_pair_type =
      interner.GetTupleType({boolean_type, boolean_type});
  REQUIRE(boolean_pair_type != nullptr);
  REQUIRE(boolean_pair_type != integer_pair_type);

  pink::Type::Pointer int_bool_pair =
      interner.GetTupleType({integer_type, boolean_type});
  REQUIRE(int_bool_pair != nullptr);
  REQUIRE(int_bool_pair != integer_pair_type);
  REQUIRE(int_bool_pair != boolean_pair_type);

  // FunctionType equality
  pink::Type::Pointer integer_binop_function =
      interner.GetFunctionType(integer_type, {integer_type, integer_type});
  REQUIRE(integer_binop_function != nullptr);
  REQUIRE(integer_binop_function != integer_type);

  pink::Type::Pointer boolean_binop_function =
      interner.GetFunctionType(boolean_type, {boolean_type, boolean_type});
  REQUIRE(boolean_binop_function != nullptr);
  REQUIRE(boolean_binop_function != integer_binop_function);

  pink::Type::Pointer integer_unop_function =
      interner.GetFunctionType(integer_type, {integer_type});
  REQUIRE(integer_unop_function != nullptr);
  REQUIRE(integer_unop_function != integer_binop_function);

  pink::Type::Pointer boolean_unop_function =
      interner.GetFunctionType(boolean_type, {boolean_type});
  REQUIRE(boolean_unop_function != nullptr);
  REQUIRE(boolean_unop_function != boolean_binop_function);
}