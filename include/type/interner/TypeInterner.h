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
#include <list>
#include <memory>
#include <string>

#include "type/All.h"

namespace pink {
/**
 * @brief Interns Types, so that only one instance of each unique type is
 * constructed
 */
class TypeInterner {
private:
  template <typename T> class Set {
  private:
    // we use a list so we can construct the types directly
    // within the list. There is no iterator invalidation
    // so we can safely return the address of a particular
    // type held within the list, and that Pointer will be
    // valid for the lifetime of the TypeInterner.
    // with vectors each time the vector needs to grow
    // we get a new + copy, so pointers to vector elements
    // would get out of sync as we added types to the set.
    // so using a list here allows each type to save a malloc.
    // as each node of the list is malloc'ed, but we save
    // malloc'ing a vector of unique ptrs.
    std::list<T> set;

  public:
    template <class... Args> auto Get(Args &&...args) -> T::Pointer {
      std::list<T> possible;
      possible.emplace_back(std::forward<Args>(args)...);
      for (auto &type : set) {
        if (possible.front().StrictEquals(&type)) {
          return &type;
        }
      }

      set.splice(set.end(), std::move(possible));
      return &set.back();
    }
  };

  Set<NilType>       nil_types;
  Set<BooleanType>   boolean_types;
  Set<IntegerType>   integer_types;
  Set<CharacterType> character_types;
  Set<VoidType>      void_types;
  Set<FunctionType>  function_types;
  Set<PointerType>   pointer_types;
  Set<ArrayType>     array_types;
  Set<SliceType>     slice_types;
  Set<TupleType>     tuple_types;
  Set<TypeVariable>  type_variables;

public:
  TypeInterner() noexcept                                     = default;
  ~TypeInterner()                                             = default;
  TypeInterner(const TypeInterner &other)                     = delete;
  TypeInterner(TypeInterner &&other)                          = default;
  auto operator=(const TypeInterner &other) -> TypeInterner & = delete;
  auto operator=(TypeInterner &&other) -> TypeInterner      & = default;

  auto GetNilType(Type::Annotations annotations) -> NilType::Pointer {
    return nil_types.Get(this, annotations);
  }
  auto GetBoolType(Type::Annotations annotations) -> BooleanType::Pointer {
    return boolean_types.Get(this, annotations);
  };
  auto GetIntType(Type::Annotations annotations) -> IntegerType::Pointer {
    return integer_types.Get(this, annotations);
  }
  auto GetCharacterType(Type::Annotations annotations)
      -> CharacterType::Pointer {
    return character_types.Get(this, annotations);
  }
  auto GetVoidType(Type::Annotations annotations) -> VoidType::Pointer {
    return void_types.Get(this, annotations);
  }

  auto GetFunctionType(Type::Annotations         annotations,
                       Type::Pointer             ret_type,
                       FunctionType::Arguments &&arg_types)
      -> FunctionType::Pointer {
    return function_types.Get(this, annotations, ret_type, arg_types);
  }

  auto GetPointerType(Type::Annotations annotations, Type::Pointer pointee_type)
      -> PointerType::Pointer {
    return pointer_types.Get(this, annotations, pointee_type);
  }
  auto GetSliceType(Type::Annotations annotations, Type::Pointer pointee_type)
      -> SliceType::Pointer {
    return slice_types.Get(this, annotations, pointee_type);
  }

  auto GetArrayType(Type::Annotations annotations,
                    std::size_t       size,
                    Type::Pointer     element_type) -> ArrayType::Pointer {
    return array_types.Get(this, annotations, size, element_type);
  }

  auto GetTupleType(Type::Annotations     annotations,
                    TupleType::Elements &&elements) -> TupleType::Pointer {
    return tuple_types.Get(this, annotations, elements);
  }

  auto GetTextType(Type::Annotations annotations, std::size_t length)
      -> ArrayType::Pointer {
    // text is the type of literal strings.
    // string literals are (Constant, InMemory,
    // non-Temporary, Comptime) ArrayTypes of
    // (Constant, InMemory, Non-Temporary, Comptime)
    // Characters.
    return GetArrayType(annotations, length, GetCharacterType(annotations));
  }

  auto GetTypeVariable(Type::Annotations annotations, InternedString identifier)
      -> TypeVariable::Pointer {
    return type_variables.Get(this, annotations, identifier);
  }
};

} // namespace pink

// ---
