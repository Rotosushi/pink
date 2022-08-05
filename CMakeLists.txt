
# general project properties
cmake_minimum_required(VERSION 3.10)

# we need to use LANGUAGES C CXX to get cmake to stop complaining 
# about not finding the termInfo package. 
project(pink LANGUAGES C CXX VERSION 0.1 DESCRIPTION "A small Programming Language, Exploring Functions as Values")

set(CMAKE_C_STANDARD 11)
# since this project uses some c++17 features
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options(-Wall -O0 -gdwarf-4 -fstandalone-debug)

cmake_policy(SET CMP0076 NEW)

# finding and setting up LLVM to be linked into this project
find_package(LLVM 15 REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})

message(STATUS "llvm include directories ${LLVM_INCLUDE_DIRS}")

message (STATUS "llvm library directories ${LLVM_LIBRARY_DIRS}")

separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

# link against all llvm libraries available.
execute_process(
	COMMAND llvm-config --libs 
	OUTPUT_VARIABLE llvm_libs
	OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
	COMMAND llvm-config --cxxflags
	OUTPUT_VARIABLE llvm_cxxflags
	OUTPUT_STRIP_TRAILING_WHITESPACE
)

add_compile_options(${llvm_cxxflags})

execute_process(
	COMMAND llvm-config --ldflags
	OUTPUT_VARIABLE llvm_ldflags
	OUTPUT_STRIP_TRAILING_WHITESPACE
)

separate_arguments(llvm_ldflags)

execute_process(
  COMMAND llvm-config --system-libs
  OUTPUT_VARIABLE llvm_syslibs
  OUTPUT_STRIP_TRAILING_WHITESPACE  
)

separate_arguments(llvm_syslibs)




# use re2c to generate 'Lexer.cpp' from 'Lexer.re',
# Lexer.cpp is then needed to build the static library
add_custom_command(
	COMMAND re2c src/front/Lexer.re -o src/front/Lexer.cpp
	OUTPUT ${pink_SOURCE_DIR}/src/front/Lexer.cpp
	DEPENDS ${pink_SOURCE_DIR}/src/front/Lexer.re
	WORKING_DIRECTORY ${pink_SOURCE_DIR}
)



# building the actual project
include_directories(include)

configure_file(include/PinkConfig.h.in ../include/PinkConfig.h)


add_library(libpink STATIC
		    src/ast/Assignment.cpp
		    src/ast/Ast.cpp
		    src/ast/Bind.cpp
		    src/ast/Binop.cpp
		    src/ast/Block.cpp
		    src/ast/Function.cpp
		    src/ast/Bool.cpp
		    src/ast/Int.cpp
		    src/ast/Nil.cpp
		    src/ast/Unop.cpp
		    src/ast/Variable.cpp
		    src/ast/VarRef.cpp
        src/ast/Application.cpp
        src/aux/CLIOptions.cpp
		    src/aux/Environment.cpp
		    src/aux/Error.cpp
		    src/aux/Location.cpp
		    src/aux/Outcome.cpp
		    src/aux/StringInterner.cpp
		    src/aux/SymbolTable.cpp
		    src/aux/TypeInterner.cpp
        src/aux/Flags.cpp
        src/ops/BinopCodegen.cpp
		    src/ops/BinopLiteral.cpp
		    src/ops/BinopTable.cpp
		    src/ops/UnopCodegen.cpp
		    src/ops/UnopLiteral.cpp
		    src/ops/UnopTable.cpp
		    src/kernel/BinopPrimitives.cpp
		    src/kernel/UnopPrimitives.cpp
        src/kernel/Cast.cpp
        src/type/Type.cpp
		    src/type/BoolType.cpp
		    src/type/IntType.cpp
		    src/type/NilType.cpp
		    src/type/FunctionType.cpp
        src/type/PointerType.cpp
        src/front/Lexer.cpp 
		    src/front/Token.cpp
		    src/front/Parser.cpp
		    src/core/Compile.cpp
        src/core/Link.cpp
		   )
			   

set_target_properties(libpink PROPERTIES SOVERSION ${PROJECT_VERSION})

set_target_properties(libpink PROPERTIES PUBLIC_HEADER include/libpink.h)

set_target_properties(libpink PROPERTIES LIBRARY_OUTPUT_DIRECTORY lib)


set_property(
	TARGET libpink 
	APPEND
	PROPERTY ADDITIONAL_CLEAN_FILES src/front/Lexer.cpp
)

target_link_options(libpink PUBLIC ${llvm_ldflags} ${llvm_syslibs} )

target_include_directories(libpink PUBLIC include)

target_link_libraries(libpink
${LLVM_LIBRARY_DIRS}/liblldELF.a
${LLVM_LIBRARY_DIRS}/liblldCommon.a
${llvm_libs}
)
			   
add_executable(pink src/Pink.cpp)

#target_link_options(pink PUBLIC --verbose)

target_link_libraries(pink
libpink
)

target_include_directories(pink PUBLIC
						   "${PROJECT_BINARY_DIR}"
						   include
						  )
						  
#TODO: figure out how to build the tests alongside the main project,
#		but only when the correct argument is passed into the call to 
#		cmake.
add_subdirectory(test)

