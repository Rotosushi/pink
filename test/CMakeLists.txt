

cmake_minimum_required(VERSION 3.10)

#set (CMAKE_C_COMPILER /usr/bin/clang-14)
#set (CMAKE_CXX_COMPILER /usr/bin/clang-14)

project(pink-test VERSION 0.1)

set (CMAKE_C_STANDARD 11)
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED True)

cmake_policy(SET CMP0076 NEW)

set(CMAKE_EXPORT_COMPILE_COMMANDS True)

add_compile_options(-O0 -gdwarf-4 -Wall -Wextra)

find_package(LLVM 16 REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

include_directories(include ../include)

add_executable(pink-test source/TestMain.cpp)

target_sources(pink-test PUBLIC 
	source/Test.cpp
	source/ast/TestAssignment.cpp
	source/ast/TestAstAndNil.cpp
	source/ast/TestBind.cpp
	source/ast/TestBinop.cpp
	source/ast/TestBool.cpp
	source/ast/TestInt.cpp
	source/ast/TestUnop.cpp
	source/ast/TestVariable.cpp
	source/ast/TestBlock.cpp
	source/ast/TestFunction.cpp
	source/ast/TestApplication.cpp
	source/ast/TestArray.cpp
	source/ast/TestTuple.cpp
	source/ast/TestConditional.cpp
	source/ast/TestDot.cpp
	source/ast/TestWhile.cpp

	source/aux/TestEnvironment.cpp
	source/aux/TestError.cpp
	source/aux/TestOutcome.cpp
	source/aux/TestStringInterner.cpp
	source/aux/TestSymbolTable.cpp
	source/aux/TestTypeInterner.cpp
	source/aux/TestFlags.cpp
	source/aux/TestLocation.cpp

	source/ops/TestBinopCodegen.cpp
	source/ops/TestBinopLiteral.cpp
	source/ops/TestBinopTable.cpp
	source/ops/TestUnopCodegen.cpp
	source/ops/TestUnopLiteral.cpp
	source/ops/TestUnopTable.cpp

	source/type/TestTypeAndNilType.cpp
	source/type/TestIntType.cpp
	source/type/TestBoolType.cpp
	source/type/TestFunctionType.cpp
	source/type/TestArrayType.cpp
	source/type/TestPointerType.cpp
	source/type/TestVoidType.cpp
	source/type/TestTupleType.cpp

	source/kernel/TestBinopPrimitives.cpp
	source/kernel/TestUnopPrimitives.cpp

	source/front/TestToken.cpp
	source/front/TestLexer.cpp
	source/front/TestParser.cpp

	source/support/Common.cpp

	source/core/TestTypecheck.cpp
	source/core/TestCodegen.cpp
)


target_link_libraries(pink-test libpink)

add_link_options(-gdwarf-4)

target_include_directories(pink-test PUBLIC
						   "${PROJECT_BINARY_DIR}"
						  )



