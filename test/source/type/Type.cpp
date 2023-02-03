#include "catch2/catch_test_macros.hpp"

#include "type/All.h"

TEST_CASE("type/ArrayType", "[unit][type]") {
  auto   integer_type = std::make_unique<pink::IntegerType>();
  size_t array_size   = 2;
  auto  *element_type = integer_type.get();
  std::unique_ptr<pink::Type> type =
      std::make_unique<pink::ArrayType>(array_size, element_type);
  REQUIRE(type->GetKind() == pink::Type::Kind::Array);
  REQUIRE(llvm::isa<pink::ArrayType>(type));
  pink::ArrayType::Pointer array_type =
      llvm::dyn_cast<pink::ArrayType>(type.get());
  REQUIRE(array_type != nullptr);
  REQUIRE(array_type->GetElementType() == element_type);
  REQUIRE(array_type->GetSize() == array_size);
}

TEST_CASE("type/BooleanType", "[unit][type]") {
  std::unique_ptr<pink::Type> type = std::make_unique<pink::BooleanType>();
  REQUIRE(type->GetKind() == pink::Type::Kind::Boolean);
  REQUIRE(llvm::isa<pink::BooleanType>(type));
  REQUIRE(llvm::dyn_cast<pink::BooleanType>(type.get()) != nullptr);
}

TEST_CASE("type/CharacterType", "[unit][type]") {
  std::unique_ptr<pink::Type> type = std::make_unique<pink::CharacterType>();
  REQUIRE(type->GetKind() == pink::Type::Kind::Character);
  REQUIRE(llvm::isa<pink::CharacterType>(type));
  REQUIRE(llvm::dyn_cast<pink::CharacterType>(type.get()) != nullptr);
}

TEST_CASE("type/FunctionType", "[unit][type]") {
  auto  integer_type = std::make_unique<pink::IntegerType>();
  auto *return_type  = integer_type.get();
  pink::FunctionType::Arguments argument_types = {integer_type.get(),
                                                  integer_type.get()};
  std::unique_ptr<pink::Type>   type =
      std::make_unique<pink::FunctionType>(return_type, argument_types);
  REQUIRE(type->GetKind() == pink::Type::Kind::Function);
  REQUIRE(llvm::isa<pink::FunctionType>(type));
  pink::FunctionType::Pointer function_type =
      llvm::dyn_cast<pink::FunctionType>(type.get());
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
  std::unique_ptr<pink::Type> type = std::make_unique<pink::IntegerType>();
  REQUIRE(type->GetKind() == pink::Type::Kind::Integer);
  REQUIRE(llvm::isa<pink::IntegerType>(type));
  REQUIRE(llvm::dyn_cast<pink::IntegerType>(type.get()) != nullptr);
}

TEST_CASE("type/NilType", "[unit][type]") {
  std::unique_ptr<pink::Type> type = std::make_unique<pink::NilType>();
  REQUIRE(type->GetKind() == pink::Type::Kind::Nil);
  REQUIRE(llvm::isa<pink::NilType>(type));
  REQUIRE(llvm::dyn_cast<pink::NilType>(type.get()) != nullptr);
}

TEST_CASE("type/PointerType", "[unit][type]") {
  auto  integer_type = std::make_unique<pink::IntegerType>();
  auto *pointee_type = integer_type.get();
  std::unique_ptr<pink::Type> type =
      std::make_unique<pink::PointerType>(pointee_type);
  REQUIRE(type->GetKind() == pink::Type::Kind::Pointer);
  REQUIRE(llvm::isa<pink::PointerType>(type));
  auto *pointer_type = llvm::dyn_cast<pink::PointerType>(type.get());
  REQUIRE(pointer_type != nullptr);
  REQUIRE(pointer_type->GetPointeeType() == pointee_type);
}

TEST_CASE("type/SliceType", "[unit][type]") {
  auto  integer_type = std::make_unique<pink::IntegerType>();
  auto *pointee_type = integer_type.get();
  std::unique_ptr<pink::Type> type =
      std::make_unique<pink::SliceType>(pointee_type);
  REQUIRE(type->GetKind() == pink::Type::Kind::Slice);
  REQUIRE(llvm::isa<pink::SliceType>(type));
  auto *slice_type = llvm::dyn_cast<pink::SliceType>(type.get());
  REQUIRE(slice_type != nullptr);
  REQUIRE(slice_type->GetPointeeType() == pointee_type);
}

TEST_CASE("type/TupleType", "[unit][type]") {
  auto  integer_type                   = std::make_unique<pink::IntegerType>();
  auto *element_type                   = integer_type.get();
  pink::TupleType::Elements   elements = {element_type, element_type};
  std::unique_ptr<pink::Type> type =
      std::make_unique<pink::TupleType>(elements);
  REQUIRE(type->GetKind() == pink::Type::Kind::Tuple);
  REQUIRE(llvm::isa<pink::TupleType>(type));
  auto *tuple_type = llvm::dyn_cast<pink::TupleType>(type.get());
  REQUIRE(tuple_type != nullptr);
  for (const auto *element : tuple_type->GetElements()) {
    REQUIRE(element == element_type);
  }
}

TEST_CASE("type/VoidType", "[unit][type]") {
  std::unique_ptr<pink::Type> type = std::make_unique<pink::VoidType>();
  REQUIRE(type->GetKind() == pink::Type::Kind::Void);
  REQUIRE(llvm::isa<pink::VoidType>(type));
  REQUIRE(llvm::dyn_cast<pink::VoidType>(type.get()) != nullptr);
}
