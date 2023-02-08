/**
 * @file TypeInterner.h
 * @brief Header for class TypeInterner
 * @version 0.1
 *
 */
#pragma once
#include <memory>
#include <string>
#include <list>

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
 * \note any atomic type essentially has no implementation
 * details, this means when we dynamically allocate them
 * they take up very little space. this is good, in theory.
 * however, since they are so small, if they are allocated
 * after other memory has been allocated, they will more
 * than likely be able to be allocated immediately after
 * any other dynamic allocation, and since these types
 * are going to be alive for the life of the TypeInterner
 * they can very easily cause heap fragmentation. so, it
 * might help performance if we could allocate all small
 * types at once such that all of the small types get allocated
 * one after another. This way, they are more likely to
 * forming a larger block of an allocation in the heap.
 * then any other allocations can occur after this block of types.
 * think about the situation where we are building up an Ast, and
 * we need a new atomic type, well, more than likely the allocation
 * for the type will go right next to whatever Ast allocations had been
 * occuring, fragmenting the Ast and Type allocations, which could build
 * to more of a slowdown if we have many small types. interleaved with
 * the Ast nodes.
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
   * @brief A list of all the unique FunctionTypes
   *
   */
  std::list<std::unique_ptr<FunctionType>> function_types;

  /**
   * @brief A list of all the unique PointerTypes
   *
   */
  std::list<std::unique_ptr<PointerType>> pointer_types;

  /**
   * @brief A list of all the unique ArrayTypes
   *
   */
  std::list<std::unique_ptr<ArrayType>> array_types;

  /**
   * @brief A list of all the unique SliceTypes
   *
   */
  std::list<std::unique_ptr<SliceType>> slice_types;

  /**
   * @brief A list of all the unique TupleTypes
   *
   */
  std::list<std::unique_ptr<TupleType>> tuple_types;

public:
  TypeInterner()                                              = default;
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

  auto GetFunctionType(Type::Pointer                  ret_type,
                       FunctionType::Arguments const &arg_types)
      -> FunctionType::Pointer;
  auto GetFunctionType(Type::Pointer             ret_type,
                       FunctionType::Arguments &&arg_types)
      -> FunctionType::Pointer;

  auto GetPointerType(Type::Pointer pointee_type) -> PointerType::Pointer;
  auto GetSliceType(Type::Pointer pointee_type) -> SliceType::Pointer;

  auto GetArrayType(std::size_t size, Type::Pointer element_type)
      -> ArrayType::Pointer;

  auto GetTupleType(TupleType::Elements const &elements) -> TupleType::Pointer;
  auto GetTupleType(TupleType::Elements &&elements) -> TupleType::Pointer;

  auto GetTextType(std::size_t length) -> ArrayType::Pointer;
};

} // namespace pink

// ---
