#include <algorithm>

#include "aux/TypeInterner.h"

#include "type/action/StructuralEquality.h"

namespace pink {

auto TypeInterner::GetNilType() -> NilType::Pointer { return &nil_type; }
auto TypeInterner::GetBoolType() -> BooleanType::Pointer { return &bool_type; }
auto TypeInterner::GetIntType() -> IntegerType::Pointer { return &int_type; }

auto TypeInterner::GetCharacterType() -> CharacterType::Pointer {
  return &character_type;
}

auto TypeInterner::GetVoidType() -> VoidType::Pointer { return &void_type; }

auto TypeInterner::GetFunctionType(Type::Pointer                  return_type,
                                   FunctionType::Arguments const &arg_types)
    -> FunctionType::Pointer {
  auto possible = std::make_unique<FunctionType>(return_type, arg_types);

  for (auto &function_type : function_types) {
    if (StructuralEquality(possible.get(), function_type.get())) {
      return function_type.get();
    }
  }

  FunctionType::Pointer result = possible.get();
  function_types.emplace_back(std::move(possible));
  return result;
}

auto TypeInterner::GetFunctionType(Type::Pointer             return_type,
                                   FunctionType::Arguments &&arg_types)
    -> FunctionType::Pointer {
  auto possible =
      std::make_unique<FunctionType>(return_type, std::move(arg_types));

  for (auto &function_type : function_types) {
    if (StructuralEquality(possible.get(), function_type.get())) {
      return function_type.get();
    }
  }

  FunctionType::Pointer result = possible.get();
  function_types.emplace_back(std::move(possible));
  return result;
}

auto TypeInterner::GetPointerType(Type::Pointer pointee_type)
    -> PointerType::Pointer {
  auto possible = std::make_unique<PointerType>(pointee_type);

  for (auto &pointer_type : pointer_types) {
    if (StructuralEquality(pointer_type.get(), possible.get())) {
      return pointer_type.get();
    }
  }

  PointerType::Pointer result = possible.get();
  pointer_types.emplace_back(std::move(possible));

  return result;
}

auto TypeInterner::GetSliceType(Type::Pointer pointee_type)
    -> SliceType::Pointer {
  auto possible = std::make_unique<SliceType>(pointee_type);

  for (auto &slice_type : slice_types) {
    if (StructuralEquality(slice_type.get(), possible.get())) {
      return slice_type.get();
    }
  }

  SliceType::Pointer result = possible.get();
  slice_types.emplace_back(std::move(possible));
  return result;
}

auto TypeInterner::GetArrayType(std::size_t size, Type::Pointer member_type)
    -> ArrayType::Pointer {
  auto possible = std::make_unique<ArrayType>(size, member_type);

  for (auto &array_type : array_types) {
    if (StructuralEquality(array_type.get(), possible.get())) {
      return array_type.get();
    }
  }

  ArrayType *result = possible.get();
  array_types.emplace_back(std::move(possible));

  return result;
}

auto TypeInterner::GetTupleType(TupleType::Elements const &member_types)
    -> TupleType::Pointer {
  auto possible = std::make_unique<TupleType>(member_types);

  for (auto &tuple_type : tuple_types) {
    if (StructuralEquality(tuple_type.get(), possible.get())) {
      return tuple_type.get();
    }
  }

  TupleType *result = possible.get();
  tuple_types.emplace_back(std::move(possible));
  return result;
}

auto TypeInterner::GetTupleType(TupleType::Elements &&member_types)
    -> TupleType::Pointer {
  auto possible = std::make_unique<TupleType>(std::move(member_types));

  for (auto &tuple_type : tuple_types) {
    if (StructuralEquality(tuple_type.get(), possible.get())) {
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
} // namespace pink
