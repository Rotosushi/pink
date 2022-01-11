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


# TODO: define a function which checks if a file has been built already
#		then does or does not compile the associated file depending.
#		This could then be used to speed up build times.
#		This wouldn't be a perfect solution, as recompilation should happen
# 		after edits to the source file, regardless of if an object file
# 		exists.

# ----------- Makefile Basic Definitions -----------
CC=clang++-13
# `llvm-config-13 -cxxflags` adds necessary flags for compiling c++ with llvm-13
CFLAGS=-g -Wall -stdlib=libstdc++ -std=c++17 -Iinclude/ -Itest/include/ -c `llvm-config-13 --cxxflags`
# `llvm-config-13 --ldflags --libs` adds necessary flags and libraries to link
# agains llvm-13, as per:
# https://stackoverflow.com/questions/53805007/compilation-failing-on-enableabibreakingchecks
LFLAGS=-g -stdlib=libstdc++ -fuse-ld=lld -std=c++17 `llvm-config-13 --ldflags --libs`

# ----------- Build Rules for the Compiler itself -----------

MAIN_OBJS=build/Pink.o
AST0_OBJS=build/Ast.o build/Nil.o build/Bool.o build/Int.o build/Variable.o
AST1_OBJS=build/Bind.o build/Binop.o build/Unop.o
AST_OBJS=$(AST0_OBJS) $(AST1_OBJS)
TYP0_OBJS=build/Type.o build/NilType.o build/IntType.o build/BoolType.o
TYPE_OBJS=$(TYP0_OBJS)
AUX0_OBJS=build/Location.o build/Error.o build/StringInterner.o build/SymbolTable.o
AUX1_OBJS=build/TypeInterner.o build/Environment.o build/Outcome.o
AUX2_OBJS=build/UnopCodegen.o build/UnopLiteral.o build/UnopTable.o
AUX3_OBJS=build/BinopCodegen.o
AUX_OBJS=$(AUX0_OBJS) $(AUX1_OBJS) $(AUX2_OBJS) $(AUX3_OBJS)
FRT0_OBJS=build/Token.o build/Lexer.o
FRNT_OBJS=$(FRT0_OBJS)

BUILD_OBJS:=$(AUX_OBJS) $(AST_OBJS) $(TYPE_OBJS) $(FRNT_OBJS)

# The final linking step which builds the compiler
pink: components main
	$(CC) $(LFLAGS) $(BUILD_OBJS) $(MAIN_OBJS) -o Pink

# The main subroutine of the compiler
main:
	$(CC) $(CFLAGS) src/Pink.cpp -o build/Pink.o

# this is the rule that builds all of the parts of the compiler
# up to the main rubroutine, so that both the main build rules
# and the unit test build rules can specify they want to build/test
# all of the components of the compiler.
components: aux ast type front

# auxilliary classes for the compiler, not conceptually tied to the
# compilation process, but necessary for bookkeeping and stitching
# the parts of the compiler together.
aux: location error outcome string_interner symbol_table aux2

location:
	$(CC) $(CFLAGS) src/aux/Location.cpp -o build/Location.o

error:
	$(CC) $(CFLAGS) src/aux/Error.cpp -o build/Error.o

outcome:
	$(CC) $(CFLAGS) src/aux/Outcome.cpp -o build/Outcome.o

string_interner:
	$(CC) $(CFLAGS) src/aux/StringInterner.cpp -o build/StringInterner.o

symbol_table:
	$(CC) $(CFLAGS) src/aux/SymbolTable.cpp -o build/SymbolTable.o

aux2: type_interner environment unop_codegen unop_literal unop_table aux3

type_interner:
	$(CC) $(CFLAGS) src/aux/TypeInterner.cpp -o build/TypeInterner.o

environment:
	$(CC) $(CFLAGS) src/aux/Environment.cpp -o build/Environment.o

unop_codegen:
	$(CC) $(CFLAGS) src/aux/UnopCodegen.cpp -o build/UnopCodegen.o

unop_literal:
	$(CC) $(CFLAGS) src/aux/UnopLiteral.cpp -o build/UnopLiteral.o

unop_table:
	$(CC) $(CFLAGS) src/aux/UnopTable.cpp -o build/UnopTable.o

aux3: binop_codegen

binop_codegen:
	$(CC) $(CFLAGS) src/aux/BinopCodegen.cpp -o build/BinopCodegen.o

# Build Rules for the AST, representing typable and interpretable statements
ast: nil bool int variable bind binop unop
	$(CC) $(CFLAGS) src/ast/Ast.cpp -o build/Ast.o

nil:
	$(CC) $(CFLAGS) src/ast/Nil.cpp -o build/Nil.o

bool:
	$(CC) $(CFLAGS) src/ast/Bool.cpp -o build/Bool.o

int:
	$(CC) $(CFLAGS) src/ast/Int.cpp -o build/Int.o

variable:
	$(CC) $(CFLAGS) src/ast/Variable.cpp -o build/Variable.o

bind:
	$(CC) $(CFLAGS) src/ast/Bind.cpp -o build/Bind.o

binop:
	$(CC) $(CFLAGS) src/ast/Binop.cpp -o build/Binop.o

unop:
	$(CC) $(CFLAGS) src/ast/Unop.cpp -o build/Unop.o

# Build Rules for the Types
type: nil_type int_type bool_type
	$(CC) $(CFLAGS) src/type/Type.cpp -o build/Type.o

nil_type:
	$(CC) $(CFLAGS) src/type/NilType.cpp -o build/NilType.o

int_type:
	$(CC) $(CFLAGS) src/type/IntType.cpp -o build/IntType.o

bool_type:
	$(CC) $(CFLAGS) src/type/BoolType.cpp -o build/BoolType.o


# builds the Frontend of the Compiler
front: token lexer

token:
	$(CC) $(CFLAGS) src/front/Token.cpp -o build/Token.o

lexer: re
	$(CC) $(CFLAGS) src/front/Lexer.cpp -o build/Lexer.o

re:
	re2c src/front/Lexer.re -o src/front/Lexer.cpp


# ----------- Build Rules for the Test Environment -----------

# a subset of the files needed to build all of the units tests, used
# so the final rule doesn't get too long
TEST_AUX0_OBJS=test/build/TestError.o test/build/TestStringInterner.o
TEST_AUX1_OBJS=test/build/TestSymbolTable.o test/build/TestTypeInterner.o
TEST_AUX2_OBJS=test/build/TestEnvironment.o test/build/TestUnopCodegen.o
TEST_AUX3_OBJS=test/build/TestOutcome.o test/build/TestUnopTable.o
TEST_AUX4_OBJS=test/build/TestUnopLiteral.o test/build/TestBinopCodegen.o
TEST_AUX_OBJS=$(TEST_AUX0_OBJS) $(TEST_AUX1_OBJS) $(TEST_AUX2_OBJS) $(TEST_AUX3_OBJS) $(TEST_AUX4_OBJS)
TEST_AST0_OBJS=test/build/TestAstAndNil.o test/build/TestBool.o test/build/TestInt.o
TEST_AST1_OBJS=test/build/TestVariable.o test/build/TestBind.o test/build/TestBinop.o
TEST_AST2_OBJS=test/build/TestUnop.o
TEST_AST_OBJS=$(TEST_AST0_OBJS) $(TEST_AST1_OBJS) $(TEST_AST2_OBJS)
TEST_TYP0_OBJS=test/build/TestTypeAndNilType.o test/build/TestIntType.o
TEST_TYP1_OBJS=test/build/TestBoolType.o
TEST_TYPE_OBJS=$(TEST_TYP0_OBJS) $(TEST_TYP1_OBJS)
TEST_FRT0_OBJS=test/build/TestLexer.o test/build/TestToken.o
TEST_FRNT_OBJS=$(TEST_FRT0_OBJS)
# the list of all the object files needed to build the main subroutine
# for the unit tests.
TEST_MAIN_OBJS=test/build/Test.o test/build/TestMain.o

# The list of all of the object files needed to build the unit tests
TEST_OBJS= $(TEST_AUX_OBJS) $(TEST_AST_OBJS) $(TEST_TYPE_OBJS) $(TEST_FRNT_OBJS)

# the final linking step of the unit tests for the compiler
test: components tests
	$(CC) $(LFLAGS) $(BUILD_OBJS) $(TEST_OBJS) $(TEST_MAIN_OBJS) -o Test


# this is the rule that collects all the subrules
# needed to build the unit tests
tests: test_main test_header test_aux test_ast test_type test_front

# This is the routine which actually handles calling all of the
# individual unit tests.
test_header:
	$(CC) $(CFLAGS) test/src/Test.cpp -o test/build/Test.o

# Defines the main subroutine which runs the unit tests
test_main:
	$(CC) $(CFLAGS) test/src/TestMain.cpp -o test/build/TestMain.o


# unit tests for the auxialliary classes used
# by the core classes of the compiler
test_aux: test_error test_outcome test_string_interner test_symbol_table test_aux2

test_error:
	$(CC) $(CFLAGS) test/src/aux/TestError.cpp -o test/build/TestError.o

test_outcome:
	$(CC) $(CFLAGS) test/src/aux/TestOutcome.cpp -o test/build/TestOutcome.o

test_string_interner:
	$(CC) $(CFLAGS) test/src/aux/TestStringInterner.cpp -o test/build/TestStringInterner.o

test_symbol_table:
	$(CC) $(CFLAGS) test/src/aux/TestSymbolTable.cpp -o test/build/TestSymbolTable.o


test_aux2: test_type_interner test_environment test_unop_codegen test_unop_literal test_unop_table test_aux3

test_type_interner:
	$(CC) $(CFLAGS) test/src/aux/TestTypeInterner.cpp -o test/build/TestTypeInterner.o

test_environment:
	$(CC) $(CFLAGS) test/src/aux/TestEnvironment.cpp -o test/build/TestEnvironment.o

test_unop_codegen:
	$(CC) $(CFLAGS) test/src/aux/TestUnopCodegen.cpp -o test/build/TestUnopCodegen.o

test_unop_literal:
	$(CC) $(CFLAGS) test/src/aux/TestUnopLiteral.cpp -o test/build/TestUnopLiteral.o

test_unop_table:
	$(CC) $(CFLAGS) test/src/aux/TestUnopTable.cpp -o test/build/TestUnopTable.o

test_aux3: test_binop_codegen

test_binop_codegen:
	$(CC) $(CFLAGS) test/src/aux/TestBinopCodegen.cpp -o test/build/TestBinopCodegen.o

# unit tests for the abstract syntax tree.
test_ast: test_bool test_int test_variable test_bind test_binop test_unop
	$(CC) $(CFLAGS) test/src/ast/TestAstAndNil.cpp -o test/build/TestAstAndNil.o

test_bool:
	$(CC) $(CFLAGS) test/src/ast/TestBool.cpp -o test/build/TestBool.o

test_int:
	$(CC) $(CFLAGS) test/src/ast/TestInt.cpp -o test/build/TestInt.o

test_variable:
	$(CC) $(CFLAGS) test/src/ast/TestVariable.cpp -o test/build/TestVariable.o

test_bind:
	$(CC) $(CFLAGS) test/src/ast/TestBind.cpp -o test/build/TestBind.o

test_binop:
	$(CC) $(CFLAGS) test/src/ast/TestBinop.cpp -o test/build/TestBinop.o

test_unop:
	$(CC) $(CFLAGS) test/src/ast/TestUnop.cpp -o test/build/TestUnop.o


# unit tests for the separate Type
test_type: test_int_type test_bool_type
	$(CC) $(CFLAGS) test/src/type/TestTypeAndNilType.cpp -o test/build/TestTypeAndNilType.o

test_int_type:
	$(CC) $(CFLAGS) test/src/type/TestIntType.cpp -o test/build/TestIntType.o

test_bool_type:
	$(CC) $(CFLAGS) test/src/type/TestBoolType.cpp -o test/build/TestBoolType.o

# unit tests for the frontend of the compiler
test_front: test_lexer test_token

test_token:
	$(CC) $(CFLAGS) test/src/front/TestToken.cpp -o test/build/TestToken.o

test_lexer:
	$(CC) $(CFLAGS) test/src/front/TestLexer.cpp -o test/build/TestLexer.o

# ----------- Auxialliary Essentialls -----------

# cleanup built object files
clean:
	rm -rf build/
	mkdir build/
	rm -rf test/build/
	mkdir test/build/

commit:
	git add include/* src/* test/include/* test/src/* docs/* Makefile TODO.txt README.md
	git commit
