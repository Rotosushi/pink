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

#include "type/ArrayType.h"
#include "type/BoolType.h"
#include "type/CharacterType.h"
#include "type/FunctionType.h"
#include "type/IntType.h"
#include "type/NilType.h"
#include "type/PointerType.h"
#include "type/SliceType.h"
#include "type/TupleType.h"
#include "type/Type.h"
#include "type/VoidType.h"

namespace pink {
/**
 * @brief Interns Types, so that only one instance of each unique type is
 * constructed
 *
 *
 */
class TypeInterner {
private:
  /**
   * @brief The one instance of a NilType
   *
   */
  std::unique_ptr<NilType> nil_type;

  /**
   * @brief The one instance of a BooleanType
   *
   */
  std::unique_ptr<BooleanType> bool_type;

  /**
   * @brief The one instance of an IntegerType
   *
   */
  std::unique_ptr<IntegerType> int_type;

  /**
   * @brief The one instance of a CharacterType
   *
   */
  std::unique_ptr<CharacterType> character_type;

  /**
   * @brief The one instance of a VoidType
   *
   */
  std::unique_ptr<VoidType> void_type;

  /**
   * @brief A vector of all the unique FunctionTypes
   *
   */
  std::vector<std::unique_ptr<FunctionType>> function_types;

  /**
   * @brief A vector of all the unique PointerTypes
   *
   */
  std::vector<std::unique_ptr<PointerType>> pointer_types;

  /**
   * @brief A vector of all the unique ArrayTypes
   *
   */
  std::vector<std::unique_ptr<ArrayType>> array_types;

  /**
   * @brief A vector of all the unique SliceTypes
   *
   */
  std::vector<std::unique_ptr<SliceType>> slice_types;

  /**
   * @brief A vector of all the unique TupleTypes
   *
   */
  std::vector<std::unique_ptr<TupleType>> tuple_types;

public:
  /**
   * @brief Construct a new Type Interner
   *
   */
  TypeInterner()                                              = default;

  /**
   * @brief We disallow a TypeInterner to be constructed from an existing
   * TypeInterner
   *
   * @param other the other Type Interner
   */
  TypeInterner(const TypeInterner &other)                     = delete;

  TypeInterner(TypeInterner &&other)                          = default;

  auto operator=(const TypeInterner &other) -> TypeInterner & = delete;

  auto operator=(TypeInterner &&other) -> TypeInterner      & = default;

  /**
   * @brief Destroy the Type Interner
   *
   */
  ~TypeInterner()                                             = default;

  /**
   * @brief Get the instance of a NilType
   *
   * @return NilType* the NilType
   */
  auto GetNilType() -> NilType::Pointer;

  /**
   * @brief Get the instance of a BooleanType
   *
   * @return BooleanType* the BooleanType
   */
  auto GetBoolType() -> BooleanType::Pointer;

  /**
   * @brief Get the instance of an IntegerType
   *
   * @return IntegerType* the IntegerType
   */
  auto GetIntType() -> IntegerType::Pointer;

  /**
   * @brief Get the instance of a Character Type
   *
   * @return CharacterType* the CharacterType
   */
  auto GetCharacterType() -> CharacterType::Pointer;

  /**
   * @brief Get the instance of a VoidType
   *
   * @return VoidType* the VoidType
   */
  auto GetVoidType() -> VoidType::Pointer;

  /**
   * @brief Get the instance of a FunctionType
   *
   * @param ret_type The return type of the FunctionType
   * @param arg_types the argument types of the FunctionType
   * @return FunctionType* the FunctionType
   */
  auto GetFunctionType(Type::Pointer                     ret_type,
                       std::vector<Type::Pointer> const &arg_types)
      -> FunctionType::Pointer;

  /**
   * @brief Get the instance of a PointerType
   *
   * @param pointee_type the pointee type of the PointerType
   * @return PointerType* the PointerType
   */
  auto GetPointerType(Type::Pointer pointee_type) -> PointerType::Pointer;

  /**
   * @brief Get the instance of a ArrayType
   *
   * @param size the size of the ArrayType
   * @param element_type the member type of the ArrayType
   * @return ArrayType* the ArrayType
   */
  auto GetArrayType(size_t size, Type::Pointer element_type)
      -> ArrayType::Pointer;

  /**
   * @brief Get the instance of a SliceType
   * @param pointee_type the type of the pointee
   * @return SliceType*
   */
  auto GetSliceType(Type::Pointer pointee_type) -> SliceType::Pointer;

  /**
   * @brief Get the instance of a TupleType
   *
   * @param member_types the member types of the TupleType
   * @return TupleType* the TupleType
   */
  auto GetTupleType(std::vector<Type::Pointer> const &member_types)
      -> TupleType::Pointer;
};

} // namespace pink

// ---
