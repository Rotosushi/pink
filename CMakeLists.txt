
# general project properties
cmake_minimum_required(VERSION 3.10)

#set(CMAKE_C_COMPILER /usr/bin/clang++-14)
#set(CMAKE_CXX_COMPILER /usr/bin/clang++-14)

# we need to use LANGUAGES C CXX to get cmake to stop complaining 
# about not finding the termInfo package. 
project(pink LANGUAGES C CXX VERSION 0.1 DESCRIPTION "A small Programming Language, Exploring Functions as Values")

set(CMAKE_C_STANDARD 11)
# since this project uses some c++20 features
set(CMAKE_CXX_STANDARD 20)
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
	COMMAND re2c source/front/Lexer.re -o source/front/Lexer.cpp
	OUTPUT ${pink_SOURCE_DIR}/source/front/Lexer.cpp
	DEPENDS ${pink_SOURCE_DIR}/source/front/Lexer.re
	WORKING_DIRECTORY ${pink_SOURCE_DIR}
)



# building the actual project
include_directories(include)

configure_file(include/PinkConfig.h.in ../include/PinkConfig.h)


add_library(common STATIC
	# the 'ast' directory is for all the classes which together 
	# comprise the representation of abstract syntax trees within
	# our compiler.
	source/ast/Application.cpp
	source/ast/Array.cpp
	source/ast/Assignment.cpp
	source/ast/Ast.cpp
	source/ast/AstToString.cpp
	source/ast/Bind.cpp
	source/ast/Binop.cpp
	source/ast/Block.cpp
	source/ast/Bool.cpp
	source/ast/Conditional.cpp
	source/ast/Dot.cpp
	source/ast/Function.cpp
	source/ast/Int.cpp
	source/ast/Nil.cpp
	source/ast/Subscript.cpp
	source/ast/Tuple.cpp
  source/ast/Typecheck.cpp
  source/ast/TypecheckVisitor.cpp
	source/ast/Unop.cpp
	source/ast/Variable.cpp
	source/ast/While.cpp
	

	# the 'type' directory is for all of the classes which together 
	# comprise the representation of types within our compiler.
	source/type/ArrayType.cpp
	source/type/BoolType.cpp
	source/type/CharacterType.cpp
	source/type/FunctionType.cpp
	source/type/IntType.cpp
	source/type/NilType.cpp
	source/type/PointerType.cpp
	source/type/SliceType.cpp
	source/type/TupleType.cpp
	source/type/Type.cpp
	source/type/VoidType.cpp


	# the 'aux' directory is for the classes which are necessary or convenient
	# for the functioning of the compiler, which are additionally small enough 
	# to fit within a single source file.
	source/aux/CLIOptions.cpp
	source/aux/Environment.cpp
	source/aux/Error.cpp
	source/aux/Location.cpp
	source/aux/Outcome.cpp
	source/aux/StringInterner.cpp
	source/aux/SymbolTable.cpp
	source/aux/TypeInterner.cpp
	source/aux/Flags.cpp

	# the 'ops' directory is for the classes which comprise the semantics
	# of binary and unary operators within the language. (minus the specialized 
	# operators like assignment '=', pointer indirection '*', and address of '&')
	source/ops/BinopCodegen.cpp
	source/ops/BinopLiteral.cpp
	source/ops/BinopTable.cpp
	source/ops/UnopCodegen.cpp
	source/ops/UnopLiteral.cpp
	source/ops/UnopTable.cpp

	# the 'front' directory is for the classes which comprise the frontend
	# of the compiler itself.
	source/front/Lexer.cpp 
	source/front/Token.cpp
	source/front/Parser.cpp
		
	# the 'kernel' directory is for files which implement functionality 
	# within the compiler which is made directly available to the programmer.
	source/kernel/ops/BinopPrimitives.cpp
	source/kernel/ops/UnopPrimitives.cpp
	source/kernel/sys/SysExit.cpp
	source/kernel/sys/SysWrite.cpp
	source/kernel/Cast.cpp
	source/kernel/StoreAggregate.cpp
	source/kernel/StoreValue.cpp
	source/kernel/LoadValue.cpp
	source/kernel/AllocateVariable.cpp 
	source/kernel/AllocateText.cpp
	source/kernel/RuntimeError.cpp
	source/kernel/SliceSubscript.cpp
	source/kernel/ArraySubscript.cpp


	# the 'support' directory is for files which define necessary or convenient 
	# subroutines, which are too small to justify their own class.
	source/support/LLVMErrorToString.cpp
	source/support/LLVMValueToString.cpp
	source/support/LLVMTypeToString.cpp 
	source/support/EmitFile.cpp
	source/support/Gensym.cpp

	source/visitor/AstVisitor.cpp
)
			   

set_target_properties(common PROPERTIES SOVERSION ${PROJECT_VERSION})

set_target_properties(common PROPERTIES PUBLIC_HEADER include/common.h)

set_target_properties(common PROPERTIES LIBRARY_OUTPUT_DIRECTORY lib)


set_property(
	TARGET common 
	APPEND
	PROPERTY ADDITIONAL_CLEAN_FILES source/front/Lexer.cpp
)

add_link_options(-gdwarf-4)

target_link_options(common PUBLIC ${llvm_ldflags} ${llvm_syslibs} -fuse-ld=lld)

target_include_directories(common PUBLIC include ${LLVM_LIBRARY_DIRS})

target_link_directories(common PUBLIC ${LLVM_LIBRARY_DIRS})

target_link_libraries(common
#${LLVM_LIBRARY_DIRS}/liblldCommon.a
#${LLVM_LIBRARY_DIRS}/liblldELF.a
${llvm_libs}
)
			   
add_executable(pink 
        # the 'core' directory is for the core files within the compiler,
        # that is, those functions which are central drivers of the program.
        source/core/Compile.cpp
        source/core/Link.cpp
        source/core/Pink.cpp
)

target_link_options(pink PUBLIC ${llvm_ldflags} ${llvm_syslibs} -fuse-ld=lld)

target_link_directories(pink PUBLIC ${LLVM_LIBRARY_DIRS})

target_link_libraries(pink
${LLVM_LIBRARY_DIRS}/liblldCommon.a
${LLVM_LIBRARY_DIRS}/liblldELF.a
common
)

target_include_directories(pink PUBLIC
						   "${PROJECT_BINARY_DIR}"
						   include
						  )
						  
#TODO: figure out how to build the tests alongside the main project,
#		but only when the correct argument is passed into the call to 
#		cmake.
add_subdirectory(test)

