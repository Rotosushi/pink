# A handrolled Makefile.
# To add another file to the complete program (say: foobar.cpp)
# 	first the associated rule must be added (foobar: )
# 	this rule compiles the single file foobar.cpp
# 	placing the completed object file into the build directory (build/foobar.o)
#	Next the object file must be added to the listing of object files
#	used in the final linking step.
# Each new file has it's associated unit test file (i.e: TestFoobar.cpp)
#	this file's associated unit test rule compiles this file,
#	then the Test and Original files object files must be added to the
#	unit test final linking step.


# ----------- Makefile Basic Definitions -----------
CC=clang++-13
# `llvm-config-13 -cxxflags` adds necessary flags for compiling c++ with llvm-13
CFLAGS=-g -std=c++17 -Iinclude/ -Itest/include/ -c `llvm-config-13 --cxxflags`
# `llvm-config-13 --ldflags --libs` adds necessary flags and libraries to link
# agains llvm-13, as per:
# https://stackoverflow.com/questions/53805007/compilation-failing-on-enableabibreakingchecks
LFLAGS=-g -std=c++17 `llvm-config-13 --ldflags --libs`

# ----------- Build Rules for the Compiler itself -----------

MAIN_OBJS=build/Pink.o
AST_OBJS=build/Ast.o
AUX_OBJS=build/Location.o build/Error.o build/StringInterner.o

BUILD_OBJS:=$(AUX_OBJS) $(AST_OBJS)

# The final linking step which builds the compiler
pink: components main
	$(CC) $(LFLAGS) $(BUILD_OBJS) $(MAIN_OBJS) -o build/Pink

# The main subroutine of the compiler
main:
	$(CC) $(CFLAGS) src/Pink.cpp -o build/Pink.o

# this is the rule that builds all of the parts of the compiler
# up to the main rubroutine, so that both the main build rules
# and the unit test build rules can specify they want to build/test
# all of the components of the compiler.
components: aux ast

# auxilliary classes for the compiler, not conceptually tied to the
# compilation process, but necessary for bookkeeping and stitching
# the parts of the compiler together.
aux: location error string_interner

location:
	$(CC) $(CFLAGS) src/Location.cpp -o build/Location.o

error:
	$(CC) $(CFLAGS) src/Error.cpp -o build/Error.o

string_interner:
	$(CC) $(CFLAGS) src/StringInterner.cpp -o build/StringInterner.o


ast: nil
	$(CC) $(CFLAGS) src/Ast.cpp -o build/Ast.o

nil:
	$(CC) $(CFLAGS) src/Nil.cpp -o build/Nil.o



# ----------- Build Rules for the Test Environment -----------

# a subset of the files needed to build all of the units tests, used
# so the final rule doesn't get too long
TEST_AUX_OBJS=test/build/TestError.o test/build/TestStringInterner.o
TEST_AST_OBJS=test/build/TestAstAndNil.o

# the list of all the object files needed to build the main subroutine
# for the unit tests.
TEST_MAIN_OBJS=test/build/Test.o test/build/TestMain.o

# The list of all of the object files needed to build the unit tests
TEST_OBJS= $(TEST_AUX_OBJS)

# the final linking step of the unit tests for the compiler
test: components tests
	$(CC) $(LFLAGS) $(BUILD_OBJS) $(TEST_OBJS) $(TEST_MAIN_OBJS) -o test/build/Pink


# this is the rule that collects all the subrules
# needed to build the unit tests
tests: test_main test_header test_aux test_ast

# This is the routine which actually handles calling all of the
# individual unit tests.
test_header:
	$(CC) $(CFLAGS) test/src/Test.cpp -o test/build/Test.o

# Defines the main subroutine which runs the unit tests
test_main:
	$(CC) $(CFLAGS) test/src/TestMain.cpp -o test/build/TestMain.o


# unit tests for the auxialliary classes used
# by the core classes of the compiler
test_aux: test_error test_string_interner

test_error:
	$(CC) $(CFLAGS) test/src/TestError.cpp -o test/build/TestError.o

test_string_interner:
	$(CC) $(CFLAGS) test/src/TestStringInterner.cpp -o test/build/TestStringInterner.o


# unit tests for the central data structure of the compiler
test_ast:
	$(CC) $(CFLAGS) test/src/TestAstAndNil.cpp -o test/build/TestAstAndNil.o




# ----------- Auxialliary Essentialls -----------

# cleanup built object files 
clean:
	rm -rf build/
	mkdir build/
	rm -rf test/build/
	mkdir test/build/
