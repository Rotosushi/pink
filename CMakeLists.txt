
# general project properties
cmake_minimum_required(VERSION 3.10)

# CMake project properties
project(pink VERSION 0.1)

set(CMAKE_C_STANDARD 11)
# since this project uses some c++17 features
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options(-Wall -g -fstandalone-debug)

cmake_policy(SET CMP0076 NEW)

# finding and setting up LLVM to be linked into this project
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

# TODO: figure out precisely which llvm components that 
# the project needs to link against to build.
# TODO: setting the variable here to 'all-targets' didn't work 
# despite the fact that all and all-targets were emmitted by 
# llvm-config --components, which according to 
# https://llvm.org/docs/CMake.html#embedding-llvm-in-your-project
# should be the way to see what is valid to place in this field.
llvm_map_components_to_libnames(llvm_libs all support core native)

# use re2c to generate 'Lexer.cpp' from 'Lexer.re'
execute_process(COMMAND re2c src/front/Lexer.re -o src/front/Lexer.cpp
				WORKING_DIRECTORY ${pink_SOURCE_DIR})


# building the actual project
include_directories(include)

configure_file(include/PinkConfig.h.in PinkConfig.h)

add_executable(pink src/Pink.cpp)

target_sources(pink PUBLIC 
			   src/ast/Assignment.cpp
			   src/ast/Ast.cpp
			   src/ast/Bind.cpp
			   src/ast/Binop.cpp
			   src/ast/Bool.cpp
			   src/ast/Int.cpp
			   src/ast/Nil.cpp
			   src/ast/Unop.cpp
			   src/ast/Variable.cpp
			   src/aux/Environment.cpp
			   src/aux/Error.cpp
			   src/aux/Location.cpp
			   src/aux/Outcome.cpp
			   src/aux/StringInterner.cpp
			   src/aux/SymbolTable.cpp
			   src/aux/TypeInterner.cpp
			   src/ops/BinopCodegen.cpp
			   src/ops/BinopLiteral.cpp
			   src/ops/BinopTable.cpp
			   src/ops/UnopCodegen.cpp
			   src/ops/UnopLiteral.cpp
			   src/ops/UnopTable.cpp
			   src/kernel/BinopPrimitives.cpp
			   src/kernel/UnopPrimitives.cpp
			   src/type/Type.cpp
			   src/type/BoolType.cpp
			   src/type/IntType.cpp
			   src/type/NilType.cpp
			   src/front/Lexer.cpp 
			   src/front/Token.cpp
			   src/front/Parser.cpp
			   )

#TODO: figure out how to build the tests alongside the main project,
#		but only when the correct argument is passed into the call to 
#		cmake.
add_subdirectory(test)

target_link_libraries(pink ${llvm_libs})

target_include_directories(pink PUBLIC
						   "${PROJECT_BINARY_DIR}"
						  )

