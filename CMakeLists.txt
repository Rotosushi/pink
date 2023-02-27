
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

message(STATUS "using c compiler: ${CMAKE_C_COMPILER}")
message(STATUS "using c++ compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "using c++ standard: ${CXX_STANDARD}")

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
  -O1 
  -gdwarf-4 
  #-flto=thin
  -Wall 
  -Wdeprecated
  -Wextra 
  -Wmost 
  -Wpedantic 
  -Wconversion 
  -Wunused 
  -Warray-bounds-pointer-arithmetic
  -Warray-parameter
  -Watomic-implicit-seq-cst
  -Wbad-function-cast
  -Wbitwise-op-parentheses
  -Wbool-operation
  -Wc++-compat 
  -Wcast-align
  -Wcast-function-type 
  -Wcast-qual 
  -Wchar-subscripts 
  -Wcomma 
  -Wcomment
  -Wcompound-token-split 
  -Wconditional-uninitialized
  -Wctad-maybe-unsupported
  -Wcustom-atomic-properties
  -Wdelete-non-virtual-dtor 
  -Wnon-virtual-dtor
  -Wdeprecated-implementations
  -Wdeprecated-writable-strings 
  -Wdisabled-macro-expansion 
  -Wdouble-promotion 
  -Wdtor-name 
  -Wduplicate-decl-specifier 
  -Wduplicate-enum 
  -Wduplicate-method-arg 
  -Wduplicate-method-match 
  -Wembedded-directive 
  -Wempty-init-stmt 
  -Wempty-translation-unit 
  -Wenum-enum-conversion 
  -Wenum-float-conversion 
  -Wextra-semi 
  -Wextra-semi-stmt 
  -Wflexible-array-extensions 
  -Wfloat-conversion 
  -Wfloat-equal 
  -Wfloat-overflow-conversion 
  -Wfloat-zero-conversion 
  -Wformat-nonliteral 
  -Wformat-pedantic 
  -Wformat-type-confusion 
  -Wglobal-constructors 
  -Wgnu 
  -Wheader-hygiene 
  -Wimplicit-fallthrough 
  -Wimplicit-int-conversion 
  -Wimplicit-int-float-conversion 
  -Winfinite-recursion 
  -Wint-in-bool-context 
  -Wkeyword-macro 
  -Wlanguage-extension-token 
  -Wlogical-op-parentheses 
  -Wloop-analysis 
  -Wmain 
  -Wmove 
  -Wmalformed-warning-check 
  -Wmicrosoft 
  -Wmisleading-indentation 
  -Wmismatched-tags 
  -Wmissing-braces 
  -Wmissing-field-initializers 
  -Wmissing-method-return-type 
  -Wmissing-noreturn 
  -Wnested-anon-types 
  -Wold-style-cast 
  -Woption-ignored 
  -Wover-aligned 
  -Woverlength-strings 
  -Woverloaded-virtual 
  -Woverriding-method-mismatch 
  -Wpacked 
  -Wparentheses 
  -Wredundant-parens 
  -Wreorder-ctor 
  -Werror
  #-fsanitize=address
  #-fsanitize=undefined
  -fno-omit-frame-pointer
)

add_link_options(
  #-flto=thin
  -gdwarf-4
  #-fsanitize=address
  #-fsanitize=undefined
  -fno-omit-frame-pointer
)

find_package(Catch2 3 REQUIRED)

# finding and setting up LLVM to be linked into this project
find_package(LLVM 17 REQUIRED CONFIG)

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

set(build_type $<$<CONFIG:Debug>:DEBUG_BUILD>)
if (build_type STREQUAL DEBUG_BUILD)
  set(DEBUG 1)
else() 
  set(DEBUG 0)
endif()

configure_file(include/PinkConfig.h.in ../include/PinkConfig.h)


add_library(common STATIC
	# the 'ast' directory is for all the classes which together 
	# comprise the representation of abstract syntax trees within
	# our compiler.
  source/ast/visitor/AstVisitor.cpp

  source/ast/action/ToString.cpp
  source/ast/action/Typecheck.cpp
  source/ast/action/Codegen.cpp
	

	# the 'type' directory is for all of the classes which together 
	# comprise the representation of types within our compiler.
  source/type/visitor/TypeVisitor.cpp

  source/type/action/StructuralEquality.cpp
  source/type/action/ToString.cpp
  source/type/action/ToLLVM.cpp


	# the 'aux' directory is for the classes which are necessary or convenient 
	# for the functioning of the compiler, which are additionally small enough 
	# to fit within a single source file.
	source/aux/CLIOptions.cpp
	source/aux/Environment.cpp
	source/aux/Error.cpp
	source/aux/TypeInterner.cpp
	source/aux/InternalFlags.cpp

	# the 'ops' directory is for the classes which comprise the semantics
	# of binary and unary operators within the language. (minus the specialized 
	# operators like assignment '=', member access '.', and subscript '[]')

	# the 'front' directory is for the classes which comprise the frontend
	# of the compiler itself.
	source/front/Lexer.cpp 
	source/front/Token.cpp
	source/front/Parser.cpp


	# the 'kernel' directory is for files which implement functionality 
	# of the language itself
	source/runtime/ops/BinopPrimitives.cpp
	source/runtime/ops/UnopPrimitives.cpp
	source/runtime/sys/SysExit.cpp
	source/runtime/sys/SysWrite.cpp
	source/runtime/StaticCast.cpp
	source/runtime/StoreAggregate.cpp
	source/runtime/StoreValue.cpp
	source/runtime/LoadValue.cpp
	source/runtime/AllocateVariable.cpp
	source/runtime/AllocateText.cpp
	source/runtime/RuntimeError.cpp
	source/runtime/SliceSubscript.cpp
	source/runtime/ArraySubscript.cpp


	# the 'support' directory is for files which define small, 
  # self-contained, and necessary or convienent header files
	source/support/LLVMErrorToString.cpp
	source/support/LLVMValueToString.cpp
	source/support/LLVMTypeToString.cpp

  # the 'visitor' directory is for the visitor interfaces
  source/visitor/VisitorResult.cpp 
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
  # the 'core' directory is for the central driver functions.
  source/core/Compile.cpp
  source/core/Link.cpp
  source/core/Optimize.cpp
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
  "${PROJECT_BINARY_DIR}"
  include
)

add_executable(tests 

  test/source/core/main.cpp

  test/source/ast/Ast.cpp
  test/source/ast/Typecheck.cpp

  test/source/type/Type.cpp

  test/source/aux/CLIOptions.cpp
  test/source/aux/Error.cpp
  test/source/aux/InternalFlags.cpp
  test/source/aux/Location.cpp
  test/source/aux/Outcome.cpp
  test/source/aux/StringInterner.cpp
  test/source/aux/ScopeStack.cpp
  test/source/aux/TypeInterner.cpp

  test/source/front/Token.cpp
  test/source/front/Lexer.cpp
  test/source/front/Parser.cpp

  test/source/ops/Binops.cpp
  test/source/ops/Unops.cpp


)

target_include_directories(tests PUBLIC 
  include
)

target_link_libraries(tests
PRIVATE Catch2::Catch2
PUBLIC common
)

include(CTest)
include(Catch)
catch_discover_tests(tests)

