

cmake_minimum_required(VERSION 3.10)

#set (CMAKE_C_COMPILER /usr/bin/clang-14)
#set (CMAKE_CXX_COMPILER /usr/bin/clang-14)

project(pink-test VERSION 0.1)

set (CMAKE_C_STANDARD 11)
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED True)

cmake_policy(SET CMP0076 NEW)

add_compile_options(-O0 -gdwarf-4 -Wall -Wextra)

find_package(LLVM 16 REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

include_directories(include ../include)

add_executable(pink-test src/TestMain.cpp)

target_sources(pink-test PUBLIC 
	src/Test.cpp
	src/ast/TestAssignment.cpp
	src/ast/TestAstAndNil.cpp
	src/ast/TestBind.cpp
	src/ast/TestBinop.cpp
	src/ast/TestBool.cpp
	src/ast/TestInt.cpp
	src/ast/TestUnop.cpp
	src/ast/TestVariable.cpp
	src/ast/TestBlock.cpp
	src/ast/TestFunction.cpp
	src/ast/TestApplication.cpp
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
	src/type/TestFunctionType.cpp
	src/kernel/TestBinopPrimitives.cpp
	src/kernel/TestUnopPrimitives.cpp
	src/front/TestToken.cpp
	src/front/TestLexer.cpp
	src/front/TestParser.cpp
	src/core/TestFirstPhase.cpp
	src/core/TestBasics.cpp
)


target_link_libraries(pink-test libpink)

add_link_options(-gdwarf-4)

target_include_directories(pink-test PUBLIC
						   "${PROJECT_BINARY_DIR}"
						  )



