
# general project properties
cmake_minimum_required(VERSION 3.10)

# we need to use LANGUAGES C CXX to get cmake to stop complaining 
# about not finding the termInfo package. 
project(pink LANGUAGES C CXX VERSION 0.1 DESCRIPTION "")

set(CMAKE_C_STANDARD 11)
# since this project uses some c++20 features
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

cmake_policy(SET CMP0076 NEW)

message(STATUS "using c++ compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "using c++ standard: ${CXX_STANDARD}")
message(STATUS "build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "build directory: ${PROJECT_BINARY_DIR}")

set(CMAKE_EXPORT_COMPILE_COMMANDS True)

# TODO: split build of project and build of tests 

# TODO: split project build into a CMakeLists within 
#       source, and tests build into a CMakeLists within 
#       test, then this CMakeLists simply sets up global 
#       state and applies the other two CMakeLists.

# TODO: split debug with all sanitizers, -01, etc.
# and release with no sanitizers, -O3, etc. 
# TODO: check that the warnings enabled work for 
# the compiler being used to build the project.
add_compile_options(
  #-flto=thin
  -Wall 
  -Wdeprecated
  -Wextra 
  -Wpedantic 
  -Wconversion 

  -Warray-bounds-pointer-arithmetic
  -Warray-parameter
  -Wcomment
  -Wconditional-uninitialized
  -Wdelete-non-virtual-dtor 
  -Wnon-virtual-dtor
  -Woverloaded-virtual 
  -Woverriding-method-mismatch 
  -Wfloat-conversion
  
  #-Werror
  #-fsanitize=address
  #-fsanitize=undefined
  -fno-omit-frame-pointer
)

add_link_options(
  #-flto=thin
  #-gdwarf-4
  #-fsanitize=address
  #-fsanitize=undefined
  -fno-omit-frame-pointer
)

find_package(Catch2 3 REQUIRED)

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
	COMMAND llvm-config --libs 
	OUTPUT_VARIABLE llvm_libs 
	OUTPUT_STRIP_TRAILING_WHITESPACE 
)

separate_arguments(llvm_libs)

# retrieved from llvm-config --cxxflags
add_compile_options(
  -I/usr/local/include 
  -fno-exceptions
  -fno-rtti 
  -D_GNU_SOURCE
  -D_DEBUG 
  -D__STDC_CONSTANT_MACROS
  -D__STDC_FORMAT_MACROS
  -D__STDC_LIMIT_MACROS
)

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

if ($<CONFIG:Debug>)
  set(DEBUG 1)
else() 
  set(DEBUG 0)
endif()

configure_file(include/PinkConfig.h.in ../include/PinkConfig.h)


add_library(common
	# the 'ast' directory is for all the classes which together 
	# comprise the representation of abstract syntax trees within
	# our compiler.
  source/ast/action/ToString.cpp
  source/ast/action/Typecheck.cpp
  source/ast/action/Codegen.cpp

	# the 'type' directory is for all of the classes which together 
	# comprise the representation of types within our compiler.
  source/type/action/PolymorphicEquality.cpp
  source/type/action/StructuralEquality.cpp
  source/type/action/Substitution.cpp 
  source/type/action/ToString.cpp
  source/type/action/ToLLVM.cpp
  source/type/interner/TypeInterner.cpp

	# the 'aux' directory is for the classes which are necessary or convenient 
	# for the functioning of the compiler, which are additionally small enough 
	# to fit within a single source file.
	source/aux/CLIOptions.cpp
	source/aux/Environment.cpp
	source/aux/Error.cpp
	
	# the 'ops' directory is for the classes which comprise the semantics
	# of binary and unary operators within the language. (minus the specialized 
	# operators like assignment '=', member access '.', and subscript '[]')
  source/ops/BinopPrimitives.cpp 
  source/ops/UnopPrimitives.cpp

	# the 'front' directory is for the classes which comprise the frontend
	# of the compiler itself.
	source/front/Lexer.cpp 
	source/front/Token.cpp
	source/front/Parser.cpp

	# the 'support' directory is for files which define small, 
  # self-contained, and necessary or convienent header files

  # the 'core' directory is for the central driver functions.
  source/core/Compile.cpp
  source/core/Link.cpp
)


set_target_properties(common PROPERTIES SOVERSION ${PROJECT_VERSION})

set_target_properties(common PROPERTIES PUBLIC_HEADER include/common.h)

set_target_properties(common PROPERTIES LIBRARY_OUTPUT_DIRECTORY lib)


set_property(
	TARGET common 
	APPEND
	PROPERTY ADDITIONAL_CLEAN_FILES source/front/Lexer.cpp
)

target_link_options(common PUBLIC ${llvm_ldflags} ${llvm_libs} ${llvm_syslibs} -fuse-ld=lld)

target_include_directories(common PUBLIC include ${LLVM_LIBRARY_DIRS})

target_link_directories(common PUBLIC ${LLVM_LIBRARY_DIRS})

target_link_libraries(common 
${LLVM_LIBRARY_DIRS}/liblldCommon.a
${LLVM_LIBRARY_DIRS}/liblldELF.a
)

add_executable(pink 
  source/core/main.cpp
)

target_link_options(pink PUBLIC ${llvm_ldflags} ${llvm_syslibs} -fuse-ld=lld)

target_link_directories(pink PUBLIC ${LLVM_LIBRARY_DIRS})

target_link_libraries(pink
common
${LLVM_LIBRARY_DIRS}/liblldCommon.a
${LLVM_LIBRARY_DIRS}/liblldELF.a
)

target_include_directories(pink PUBLIC
  include
)

add_executable(tests 

  test/source/core/main.cpp

  test/source/ast/Ast.cpp
  test/source/ast/Typecheck.cpp
  test/source/ast/Codegen.cpp

  test/source/type/Type.cpp
  test/source/type/TypeInterner.cpp

  test/source/aux/CLIOptions.cpp
  test/source/aux/Error.cpp
  test/source/aux/InternalFlags.cpp
  test/source/aux/Location.cpp
  test/source/aux/Outcome.cpp
  test/source/aux/StringInterner.cpp
  test/source/aux/ScopeStack.cpp

  test/source/front/Token.cpp
  test/source/front/Lexer.cpp
  test/source/front/Parser.cpp

  test/source/ops/Binops.cpp
  test/source/ops/Unops.cpp
)

target_include_directories(tests PUBLIC 
  include
  test/include
)

target_link_libraries(tests
PRIVATE Catch2::Catch2
PUBLIC common
)

include(CTest)
include(Catch)
catch_discover_tests(tests)


if ($<Config:CFG>) 
set (PINK_BUILD_DIR "${PROJECT_BINARY_DIR}/CMakeFiles")
set(PINK_LLVM_IR_FILES 
  
)

add_custom_command(
  TARGET pink POST_BUILD 
  COMMAND opt --dot-cfg-mssa=pink-cfg.dot 
  COMMAND dot -Tsvg pink-cfg.dot -o pink-cfg.svg
  DEPENDS 
VERBATIM 
)
endif()