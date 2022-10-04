
# general project properties
cmake_minimum_required(VERSION 3.10)

#set(CMAKE_C_COMPILER /usr/bin/clang++-14)
#set(CMAKE_CXX_COMPILER /usr/bin/clang++-14)

# we need to use LANGUAGES C CXX to get cmake to stop complaining 
# about not finding the termInfo package. 
project(pink LANGUAGES C CXX VERSION 0.1 DESCRIPTION "A small Programming Language, Exploring Functions as Values")

set(CMAKE_C_STANDARD 11)
# since this project uses some c++17 features
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

message(STATUS "using c compiler: ${CMAKE_C_COMPILER}")
message(STATUS "using c++ compiler: ${CMAKE_CXX_COMPILER}")

set(CMAKE_EXPORT_COMPILE_COMMANDS True)

add_compile_options(-O0 -gdwarf-4 -Wall -Wextra)

cmake_policy(SET CMP0076 NEW)

# finding and setting up LLVM to be linked into this project
find_package(LLVM 16 REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})

message(STATUS "llvm include directories ${LLVM_INCLUDE_DIRS}")

message (STATUS "llvm library directories ${LLVM_LIBRARY_DIRS}")

separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

# link against all llvm libraries available.
execute_process(
	COMMAND llvm-config --libfiles 
	OUTPUT_VARIABLE llvm_libs
	OUTPUT_STRIP_TRAILING_WHITESPACE
)

separate_arguments(llvm_libs)

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
	# the 'ast' directory is for all the classes which together 
	# comprise the representation of abstract syntax trees within
	# our compiler.
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
	src/ast/Application.cpp
	src/ast/Array.cpp
	src/ast/Tuple.cpp
	src/ast/Conditional.cpp
	src/ast/While.cpp
	src/ast/Dot.cpp

	# the 'type' directory is for all of the classes which together 
	# comprise the representation of types within our compiler.
	src/type/Type.cpp
	src/type/BoolType.cpp
	src/type/IntType.cpp
	src/type/NilType.cpp
	src/type/VoidType.cpp
	src/type/FunctionType.cpp
	src/type/PointerType.cpp
	src/type/ArrayType.cpp
	src/type/TupleType.cpp


	# the 'aux' directory is for the classes which are necessary or convenient
	# for the functioning of the compiler, which are additionally small enough 
	# to fit within a single source file.
	src/aux/CLIOptions.cpp
	src/aux/Environment.cpp
	src/aux/Error.cpp
	src/aux/Location.cpp
	src/aux/Outcome.cpp
	src/aux/StringInterner.cpp
	src/aux/SymbolTable.cpp
	src/aux/TypeInterner.cpp
	src/aux/Flags.cpp

	# the 'ops' directory is for the classes which comprise the semantics
	# of binary and unary operators within the language. (minus the specialized 
	# operators like assignment '=', pointer indirection '*', and address of '&')
	# (aside: all operator classes were once in 'aux' however that clutters up 
	# 'aux' so any semantics which require multiple classes working in concert 
	# to acheive a single purpose will be placed in their own directory)
	src/ops/BinopCodegen.cpp
	src/ops/BinopLiteral.cpp
	src/ops/BinopTable.cpp
	src/ops/UnopCodegen.cpp
	src/ops/UnopLiteral.cpp
	src/ops/UnopTable.cpp

	# the 'front' directory is for the classes which comprise the frontend
	# of the compiler itself.
	src/front/Lexer.cpp 
	src/front/Token.cpp
	src/front/Parser.cpp
		
	# the 'kernel' directory is for files which implement functionality 
	# within the compiler which is made directly available to the programmer.
	src/kernel/BinopPrimitives.cpp
	src/kernel/UnopPrimitives.cpp
	src/kernel/Cast.cpp
	src/kernel/StoreAggregate.cpp

	# the 'support' directory is for files which define necessary or convenient 
	# subroutines, which are too small to justify their own class.
	src/support/LLVMErrorToString.cpp
	src/support/LLVMValueToString.cpp
	src/support/LLVMTypeToString.cpp 
	src/support/EmitFile.cpp

)
			   

set_target_properties(libpink PROPERTIES SOVERSION ${PROJECT_VERSION})

set_target_properties(libpink PROPERTIES PUBLIC_HEADER include/libpink.h)

set_target_properties(libpink PROPERTIES LIBRARY_OUTPUT_DIRECTORY lib)


set_property(
	TARGET libpink 
	APPEND
	PROPERTY ADDITIONAL_CLEAN_FILES src/front/Lexer.cpp
)

add_link_options(-gdwarf-4)

target_link_options(libpink PUBLIC ${llvm_ldflags} ${llvm_syslibs} -fuse-ld=lld)

target_include_directories(libpink PUBLIC include ${LLVM_LIBRARY_DIRS})

target_link_directories(libpink PUBLIC ${LLVM_LIBRARY_DIRS})

target_link_libraries(libpink
#${LLVM_LIBRARY_DIRS}/liblldCommon.a
#${LLVM_LIBRARY_DIRS}/liblldELF.a
${llvm_libs}
)
			   
add_executable(pink 
        # the 'core' directory is for the core files within the compiler,
        # that is, those functions which are central drivers of the program.
        src/core/Compile.cpp
        src/core/Link.cpp
        src/core/Pink.cpp
)

target_link_options(pink PUBLIC ${llvm_ldflags} ${llvm_syslibs} -fuse-ld=lld)

target_link_directories(pink PUBLIC ${LLVM_LIBRARY_DIRS})

target_link_libraries(pink
${LLVM_LIBRARY_DIRS}/liblldCommon.a
${LLVM_LIBRARY_DIRS}/liblldELF.a
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

