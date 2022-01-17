

cmake_minimum_required(VERSION 3.10)

project(pink-test VERSION 0.1)

set (CMAKE_C_STANDARD 11)
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED True)

set (CMAKE_C_COMPILER clang)
set (CMAKE_CXX_COMPILER clang++)

cmake_policy(SET CMP0076 NEW)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

llvm_map_components_to_libnames(llvm_libs all support core native)

include_directories(include ../include)

add_executable(pink-test src/TestMain.cpp)


target_sources(pink-test PUBLIC 
			   ../src/ast/Assignment.cpp
			   ../src/ast/Ast.cpp
			   ../src/ast/Bind.cpp
			   ../src/ast/Binop.cpp
			   ../src/ast/Bool.cpp
			   ../src/ast/Int.cpp
			   ../src/ast/Nil.cpp
			   ../src/ast/Unop.cpp
			   ../src/ast/Variable.cpp
			   ../src/aux/Environment.cpp
			   ../src/aux/Error.cpp
			   ../src/aux/Location.cpp
			   ../src/aux/Outcome.cpp
			   ../src/aux/StringInterner.cpp
			   ../src/aux/SymbolTable.cpp
			   ../src/aux/TypeInterner.cpp
			   ../src/ops/BinopCodegen.cpp
			   ../src/ops/BinopLiteral.cpp
			   ../src/ops/BinopTable.cpp
			   ../src/ops/UnopCodegen.cpp
			   ../src/ops/UnopLiteral.cpp
			   ../src/ops/UnopTable.cpp
			   ../src/kernel/BinopPrimitives.cpp
			   ../src/kernel/UnopPrimitives.cpp
			   ../src/type/Type.cpp
			   ../src/type/BoolType.cpp
			   ../src/type/IntType.cpp
			   ../src/type/NilType.cpp
			   ../src/front/Lexer.cpp 
			   ../src/front/Token.cpp
			   ../src/front/Parser.cpp
			   src/Test.cpp
			   src/ast/TestAssignment.cpp
			   src/ast/TestAstAndNil.cpp
			   src/ast/TestBind.cpp
			   src/ast/TestBinop.cpp
			   src/ast/TestBool.cpp
			   src/ast/TestInt.cpp
			   src/ast/TestUnop.cpp
			   src/ast/TestVariable.cpp
			   src/aux/TestEnvironment.cpp
			   src/aux/TestError.cpp
			   src/aux/TestOutcome.cpp
			   src/aux/TestStringInterner.cpp
			   src/aux/TestSymbolTable.cpp
			   src/aux/TestTypeInterner.cpp
			   src/ops/TestBinopCodegen.cpp
			   src/ops/TestBinopLiteral.cpp
			   src/ops/TestBinopTable.cpp
			   src/ops/TestUnopCodegen.cpp
			   src/ops/TestUnopLiteral.cpp
			   src/ops/TestUnopTable.cpp
			   src/type/TestTypeAndNilType.cpp
			   src/type/TestIntType.cpp
			   src/type/TestBoolType.cpp
			   src/kernel/TestBinopPrimitives.cpp
			   src/kernel/TestUnopPrimitives.cpp
			   src/front/TestToken.cpp
			   src/front/TestLexer.cpp
			   src/front/TestParser.cpp
			   )


target_link_libraries(pink-test ${llvm_libs})


target_include_directories(pink-test PUBLIC
						   "${PROJECT_BINARY_DIR}"
						  )



