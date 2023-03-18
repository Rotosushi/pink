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

#include "type/All.h"
#include "type/interner/TypeInterner.h"

TEST_CASE("type/ArrayType", "[unit][type]") {
  auto        interner     = pink::TypeInterner{};
  const auto *integer_type = interner.GetIntType();
  size_t      array_size   = 2;
  auto        type         = interner.GetArrayType(array_size, integer_type);
  REQUIRE(type->GetKind() == pink::Type::Kind::Array);
  REQUIRE(llvm::isa<pink::ArrayType>(type));
  pink::ArrayType::Pointer array_type = llvm::dyn_cast<pink::ArrayType>(type);
  REQUIRE(array_type != nullptr);
  REQUIRE(array_type->GetElementType() == integer_type);
  REQUIRE(array_type->GetSize() == array_size);
}

TEST_CASE("type/BooleanType", "[unit][type]") {
  auto interner = pink::TypeInterner{};
  auto type     = interner.GetBoolType();
  REQUIRE(type->GetKind() == pink::Type::Kind::Boolean);
  REQUIRE(llvm::isa<pink::BooleanType>(type));
  REQUIRE(llvm::dyn_cast<pink::BooleanType>(type) != nullptr);
}

TEST_CASE("type/CharacterType", "[unit][type]") {
  auto interner = pink::TypeInterner{};
  auto type     = interner.GetCharacterType();
  REQUIRE(type->GetKind() == pink::Type::Kind::Character);
  REQUIRE(llvm::isa<pink::CharacterType>(type));
  REQUIRE(llvm::dyn_cast<pink::CharacterType>(type) != nullptr);
}

TEST_CASE("type/FunctionType", "[unit][type]") {
  auto                          interner       = pink::TypeInterner{};
  const auto                   *integer_type   = interner.GetIntType();
  const auto                   *return_type    = integer_type;
  pink::FunctionType::Arguments argument_types = {integer_type, integer_type};
  const auto                   *type =
      interner.GetFunctionType(return_type,
                               pink::FunctionType::Arguments{argument_types});
  REQUIRE(type->GetKind() == pink::Type::Kind::Function);
  REQUIRE(llvm::isa<pink::FunctionType>(type));
  const auto *function_type = llvm::dyn_cast<pink::FunctionType>(type);
  REQUIRE(function_type != nullptr);
  REQUIRE(function_type->GetReturnType() == return_type);

  auto ft_cursor  = function_type->begin();
  auto ft_end     = function_type->end();
  auto arg_cursor = argument_types.begin();
  auto arg_end    = argument_types.end();
  while ((ft_cursor != ft_end) && (arg_cursor != arg_end)) {
    REQUIRE(*ft_cursor == *arg_cursor);
    ft_cursor++;
    arg_cursor++;
  }
}

TEST_CASE("type/IntegerType", "[unit][type]") {
  auto        interner = pink::TypeInterner{};
  const auto *type     = interner.GetIntType();
  REQUIRE(type->GetKind() == pink::Type::Kind::Integer);
  REQUIRE(llvm::isa<pink::IntegerType>(type));
  REQUIRE(llvm::dyn_cast<pink::IntegerType>(type) != nullptr);
}

TEST_CASE("type/NilType", "[unit][type]") {
  auto        interner = pink::TypeInterner{};
  const auto *type     = interner.GetNilType();
  REQUIRE(type->GetKind() == pink::Type::Kind::Nil);
  REQUIRE(llvm::isa<pink::NilType>(type));
  REQUIRE(llvm::dyn_cast<pink::NilType>(type) != nullptr);
}

TEST_CASE("type/PointerType", "[unit][type]") {
  auto        interner     = pink::TypeInterner{};
  const auto *integer_type = interner.GetIntType();
  const auto *type         = interner.GetPointerType(integer_type);
  REQUIRE(type->GetKind() == pink::Type::Kind::Pointer);
  REQUIRE(llvm::isa<pink::PointerType>(type));
  const auto *pointer_type = llvm::dyn_cast<pink::PointerType>(type);
  REQUIRE(pointer_type != nullptr);
  REQUIRE(pointer_type->GetPointeeType() == integer_type);
}

TEST_CASE("type/SliceType", "[unit][type]") {
  auto        interner     = pink::TypeInterner{};
  const auto *integer_type = interner.GetIntType();
  const auto *type         = interner.GetSliceType(integer_type);
  REQUIRE(type->GetKind() == pink::Type::Kind::Slice);
  REQUIRE(llvm::isa<pink::SliceType>(type));
  const auto *slice_type = llvm::dyn_cast<pink::SliceType>(type);
  REQUIRE(slice_type != nullptr);
  REQUIRE(slice_type->GetPointeeType() == integer_type);
}

TEST_CASE("type/TupleType", "[unit][type]") {
  auto                      interner     = pink::TypeInterner{};
  const auto               *integer_type = interner.GetIntType();
  pink::TupleType::Elements elements     = {integer_type, integer_type};
  const auto *type = interner.GetTupleType(pink::TupleType::Elements{elements});
  REQUIRE(type->GetKind() == pink::Type::Kind::Tuple);
  REQUIRE(llvm::isa<pink::TupleType>(type));
  const auto *tuple_type = llvm::dyn_cast<pink::TupleType>(type);
  REQUIRE(tuple_type != nullptr);
  for (const auto *element : tuple_type->GetElements()) {
    REQUIRE(element == integer_type);
  }
}

TEST_CASE("type/VoidType", "[unit][type]") {
  auto        interner = pink::TypeInterner{};
  const auto *type     = interner.GetVoidType();
  REQUIRE(type->GetKind() == pink::Type::Kind::Void);
  REQUIRE(llvm::isa<pink::VoidType>(type));
  REQUIRE(llvm::dyn_cast<pink::VoidType>(type) != nullptr);
}
