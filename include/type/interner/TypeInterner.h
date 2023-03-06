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
};

} // namespace pink

// ---
