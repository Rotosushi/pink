#include <algorithm>

#include "aux/TypeInterner.h"

#include "type/action/StructuralEquality.h"

namespace pink {

auto TypeInterner::GetNilType() -> NilType * {
  if (nil_type == nullptr) {
    nil_type = std::make_unique<NilType>();
  }

  return nil_type.get();
}

auto TypeInterner::GetBoolType() -> BooleanType * {
  if (bool_type == nullptr) {
    bool_type = std::make_unique<BooleanType>();
  }

  return bool_type.get();
}

auto TypeInterner::GetIntType() -> IntegerType * {
  if (int_type == nullptr) {
    int_type = std::make_unique<IntegerType>();
  }

  return int_type.get();
}

auto TypeInterner::GetCharacterType() -> CharacterType * {
  if (character_type == nullptr) {
    character_type = std::make_unique<CharacterType>();
  }

  return character_type.get();
}

auto TypeInterner::GetVoidType() -> VoidType * {
  if (void_type == nullptr) {
    void_type = std::make_unique<VoidType>();
  }

  return void_type.get();
}

auto TypeInterner::GetFunctionType(Type *return_type,
                                   const std::vector<Type *> &arg_types)
    -> FunctionType * {
  auto possible = std::make_unique<FunctionType>(return_type, arg_types);
  FunctionType *possible_result = possible.get();

  auto types_equal = [possible_result](std::unique_ptr<FunctionType> &type) {
    return StructuralEquality(possible_result, type.get());
  };
  auto found =
      std::find_if(function_types.begin(), function_types.end(), types_equal);

  if (found == function_types.end()) {
    function_types.emplace_back(std::move(possible));
    return possible_result;
  }
  return found->get();
}

auto TypeInterner::GetPointerType(Type *pointee_type) -> PointerType * {
  auto possible = std::make_unique<PointerType>(pointee_type);

  for (auto &pointer_type : pointer_types) {
    if (StructuralEquality(pointer_type.get(), possible.get())) {
      return pointer_type.get();
    }
  }

  PointerType *result = possible.get();
  pointer_types.emplace_back(std::move(possible));

  return result;
}

auto TypeInterner::GetArrayType(size_t size, Type *member_type) -> ArrayType * {
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

auto TypeInterner::GetSliceType(Type *pointee_type) -> SliceType * {
  auto possible = std::make_unique<SliceType>(pointee_type);
  auto *possible_result = possible.get();

  auto types_equal = [possible_result](std::unique_ptr<SliceType> &type) {
    return StructuralEquality(possible_result, type.get());
  };
  auto found =
      std::find_if(slice_types.begin(), slice_types.end(), types_equal);

  if (found == slice_types.end()) {
    slice_types.emplace_back(std::move(possible));
    return possible_result;
  }
  return found->get();
}

auto TypeInterner::GetTupleType(const std::vector<Type *> &member_types)
    -> TupleType * {
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
} // namespace pink
