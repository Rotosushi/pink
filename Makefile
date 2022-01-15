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
#		This could then be used to speed up build times during active
#		development, as we are often only working on a single new file.
#		This wouldn't be a perfect solution, as recompilation should happen
# 		after edits to the source file, regardless of if an object file
# 		exists.

# ----------- Makefile Basic Definitions -----------
CC=clang++-13
# `llvm-config-13 -cxxflags` adds necessary flags for compiling c++ with llvm-13
CFLAGS=-g -fstandalone-debug -Wall -stdlib=libstdc++ -std=c++17 -Iinclude/ -Itest/include/ -c `llvm-config-13 --cxxflags`
# `llvm-config-13 --ldflags --libs` adds necessary flags and libraries to link
# agains llvm-13, as per:
# https://stackoverflow.com/questions/53805007/compilation-failing-on-enableabibreakingchecks
LFLAGS=-g -stdlib=libstdc++ -std=c++17 -fuse-ld=lld `llvm-config-13 --ldflags --libs`

# ----------- Build Rules for the Compiler itself -----------

MAIN_OBJS=build/Pink.o
AST0_OBJS=build/Ast.o build/Nil.o build/Bool.o build/Int.o build/Variable.o
AST1_OBJS=build/Bind.o build/Binop.o build/Unop.o build/Assignment.o
AST_OBJS=$(AST0_OBJS) $(AST1_OBJS)
TYP0_OBJS=build/Type.o build/NilType.o build/IntType.o build/BoolType.o
TYPE_OBJS=$(TYP0_OBJS)
AUX0_OBJS=build/Location.o build/Error.o build/StringInterner.o build/SymbolTable.o
AUX1_OBJS=build/TypeInterner.o build/Environment.o build/Outcome.o
AUX_OBJS=$(AUX0_OBJS) $(AUX1_OBJS)
OPS0_OBJS=build/UnopCodegen.o build/UnopLiteral.o build/UnopTable.o
OPS1_OBJS=build/BinopCodegen.o build/BinopLiteral.o build/BinopTable.o
OPS_OBJS=$(OPS0_OBJS) $(OPS1_OBJS)
KRN0_OBJS=build/UnopPrimitives.o build/BinopPrimitives.o
KRNL_OBJS=$(KRN0_OBJS)
FRT0_OBJS=build/Token.o build/Lexer.o build/Parser.o
FRNT_OBJS=$(FRT0_OBJS)

BUILD_OBJS:=$(AUX_OBJS) $(OPS_OBJS) $(KRNL_OBJS) $(AST_OBJS) $(TYPE_OBJS) $(FRNT_OBJS)

# The final linking step which builds the compiler
pink: components main
	$(CC) $(LFLAGS) $(BUILD_OBJS) $(MAIN_OBJS) -o "pink"

# The main subroutine of the compiler
main:
	$(CC) $(CFLAGS) src/Pink.cpp -o build/Pink.o

# this is the rule that builds all of the parts of the compiler
# up to the main rubroutine, so that both the main build rules
# and the unit test build rules can specify they want to build/test
# all of the components of the compiler.
components: aux ops kernel ast type front

# auxilliary classes for the compiler, not conceptually tied to the
# compilation process, but necessary for bookkeeping and stitching
# the parts of the compiler together.
aux: location error outcome string_interner symbol_table type_interner environment

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

type_interner:
	$(CC) $(CFLAGS) src/aux/TypeInterner.cpp -o build/TypeInterner.o

environment:
	$(CC) $(CFLAGS) src/aux/Environment.cpp -o build/Environment.o

# classes which implement the structure of operators
ops: unop_codegen unop_literal unop_table binop_codegen binop_literal binop_table

unop_codegen:
	$(CC) $(CFLAGS) src/ops/UnopCodegen.cpp -o build/UnopCodegen.o

unop_literal:
	$(CC) $(CFLAGS) src/ops/UnopLiteral.cpp -o build/UnopLiteral.o

unop_table:
	$(CC) $(CFLAGS) src/ops/UnopTable.cpp -o build/UnopTable.o

binop_codegen:
	$(CC) $(CFLAGS) src/ops/BinopCodegen.cpp -o build/BinopCodegen.o

binop_literal:
	$(CC) $(CFLAGS) src/ops/BinopLiteral.cpp -o build/BinopLiteral.o

binop_table:
	$(CC) $(CFLAGS) src/ops/BinopTable.cpp -o build/BinopTable.o

# classes which implement the behavior of operators
kernel: unop_primitives binop_primitives

unop_primitives:
	$(CC) $(CFLAGS) src/kernel/UnopPrimitives.cpp -o build/UnopPrimitives.o

binop_primitives:
	$(CC) $(CFLAGS) src/kernel/BinopPrimitives.cpp -o build/BinopPrimitives.o

# Build Rules for the AST, representing typable statements
ast: nil bool int variable bind binop unop assignment
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
	
assignment:
	$(CC) $(CFLAGS) src/ast/Assignment.cpp -o build/Assignment.o

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
front: token lexer parser

token:
	$(CC) $(CFLAGS) src/front/Token.cpp -o build/Token.o

lexer: re
	$(CC) $(CFLAGS) src/front/Lexer.cpp -o build/Lexer.o

re:
	re2c src/front/Lexer.re -o src/front/Lexer.cpp

parser:
	$(CC) $(CFLAGS) src/front/Parser.cpp -o build/Parser.o

# ----------- Build Rules for the Test Environment -----------

# a subset of the files needed to build all of the units tests, used
# so the final rule doesn't get too long
TEST_AUX0_OBJS=test/build/TestError.o test/build/TestStringInterner.o
TEST_AUX1_OBJS=test/build/TestSymbolTable.o test/build/TestTypeInterner.o
TEST_AUX2_OBJS=test/build/TestEnvironment.o test/build/TestOutcome.o
TEST_AUX_OBJS=$(TEST_AUX0_OBJS) $(TEST_AUX1_OBJS) $(TEST_AUX2_OBJS)
TEST_OPS0_OBJS=test/build/TestUnopCodegen.o test/build/TestUnopLiteral.o test/build/TestUnopTable.o
TEST_OPS1_OBJS=test/build/TestBinopCodegen.o test/build/TestBinopLiteral.o test/build/TestBinopTable.o
TEST_OPS_OBJS=$(TEST_OPS0_OBJS) $(TEST_OPS1_OBJS)
TEST_KRN0_OBJS=test/build/TestUnopPrimitives.o test/build/TestBinopPrimitives.o
TEST_KRNL_OBJS=$(TEST_KRN0_OBJS)
TEST_AST0_OBJS=test/build/TestAstAndNil.o test/build/TestBool.o test/build/TestInt.o
TEST_AST1_OBJS=test/build/TestVariable.o test/build/TestBind.o test/build/TestBinop.o
TEST_AST2_OBJS=test/build/TestUnop.o test/build/TestAssignment.o
TEST_AST_OBJS=$(TEST_AST0_OBJS) $(TEST_AST1_OBJS) $(TEST_AST2_OBJS)
TEST_TYP0_OBJS=test/build/TestTypeAndNilType.o test/build/TestIntType.o
TEST_TYP1_OBJS=test/build/TestBoolType.o
TEST_TYPE_OBJS=$(TEST_TYP0_OBJS) $(TEST_TYP1_OBJS)
TEST_FRT0_OBJS=test/build/TestParser.o test/build/TestLexer.o test/build/TestToken.o
TEST_FRNT_OBJS=$(TEST_FRT0_OBJS)
# the list of all the object files needed to build the main subroutine
# for the unit tests.
TEST_MAIN_OBJS=test/build/Test.o test/build/TestMain.o

# The list of all of the object files needed to build the unit tests
TEST_OBJS=$(TEST_AUX_OBJS) $(TEST_OPS_OBJS) $(TEST_KRNL_OBJS) $(TEST_AST_OBJS) $(TEST_TYPE_OBJS) $(TEST_FRNT_OBJS)

# the final linking step of the unit tests for the compiler
test: components tests
	$(CC) $(LFLAGS) $(BUILD_OBJS) $(TEST_OBJS) $(TEST_MAIN_OBJS) -o "tests"


# this is the rule that collects all the subrules
# needed to build the unit tests
tests: test_main test_header test_aux test_ops test_kernel test_ast test_type test_front

# This is the routine which actually handles calling all of the
# individual unit tests.
test_header:
	$(CC) $(CFLAGS) test/src/Test.cpp -o test/build/Test.o

# Defines the main subroutine which runs the unit tests
test_main:
	$(CC) $(CFLAGS) test/src/TestMain.cpp -o test/build/TestMain.o


# unit tests for the auxialliary classes used
# by the core classes of the compiler
test_aux: test_error test_outcome test_string_interner test_symbol_table test_type_interner test_environment

test_error:
	$(CC) $(CFLAGS) test/src/aux/TestError.cpp -o test/build/TestError.o

test_outcome:
	$(CC) $(CFLAGS) test/src/aux/TestOutcome.cpp -o test/build/TestOutcome.o

test_string_interner:
	$(CC) $(CFLAGS) test/src/aux/TestStringInterner.cpp -o test/build/TestStringInterner.o

test_symbol_table:
	$(CC) $(CFLAGS) test/src/aux/TestSymbolTable.cpp -o test/build/TestSymbolTable.o

test_type_interner:
	$(CC) $(CFLAGS) test/src/aux/TestTypeInterner.cpp -o test/build/TestTypeInterner.o

test_environment:
	$(CC) $(CFLAGS) test/src/aux/TestEnvironment.cpp -o test/build/TestEnvironment.o


# unit tests for the operator classes
test_ops: test_unop_codegen test_unop_literal test_unop_table test_binop_codegen test_binop_literal test_binop_table

test_unop_codegen:
	$(CC) $(CFLAGS) test/src/ops/TestUnopCodegen.cpp -o test/build/TestUnopCodegen.o

test_unop_literal:
	$(CC) $(CFLAGS) test/src/ops/TestUnopLiteral.cpp -o test/build/TestUnopLiteral.o

test_unop_table:
	$(CC) $(CFLAGS) test/src/ops/TestUnopTable.cpp -o test/build/TestUnopTable.o

test_binop_codegen:
	$(CC) $(CFLAGS) test/src/ops/TestBinopCodegen.cpp -o test/build/TestBinopCodegen.o

test_binop_literal:
	$(CC) $(CFLAGS) test/src/ops/TestBinopLiteral.cpp -o test/build/TestBinopLiteral.o

test_binop_table:
	$(CC) $(CFLAGS) test/src/ops/TestBinopTable.cpp -o test/build/TestBinopTable.o

# The kernel defines all of the primitive operators for now.
test_kernel: test_unop_primitives test_binop_primitives

test_unop_primitives:
	$(CC) $(CFLAGS) test/src/kernel/TestUnopPrimitives.cpp -o test/build/TestUnopPrimitives.o

test_binop_primitives:
	$(CC) $(CFLAGS) test/src/kernel/TestBinopPrimitives.cpp -o test/build/TestBinopPrimitives.o

# unit tests for the abstract syntax tree.
test_ast: test_bool test_int test_variable test_bind test_binop test_unop test_assignment
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
	
test_assignment:
	$(CC) $(CFLAGS) test/src/ast/TestAssignment.cpp -o test/build/TestAssignment.o


# unit tests for the separate Type
test_type: test_int_type test_bool_type
	$(CC) $(CFLAGS) test/src/type/TestTypeAndNilType.cpp -o test/build/TestTypeAndNilType.o

test_int_type:
	$(CC) $(CFLAGS) test/src/type/TestIntType.cpp -o test/build/TestIntType.o

test_bool_type:
	$(CC) $(CFLAGS) test/src/type/TestBoolType.cpp -o test/build/TestBoolType.o

# unit tests for the frontend of the compiler
test_front: test_lexer test_token test_parser

test_token:
	$(CC) $(CFLAGS) test/src/front/TestToken.cpp -o test/build/TestToken.o

test_lexer:
	$(CC) $(CFLAGS) test/src/front/TestLexer.cpp -o test/build/TestLexer.o
	
test_parser:
	$(CC) $(CFLAGS) test/src/front/TestParser.cpp -o test/build/TestParser.o

# ----------- Auxialliary Essentials -----------

# cleanup built object files
clean:
	rm -rf build/
	rm -f "pink"
	mkdir build/
	rm -rf test/build/
	rm -f "tests"
	mkdir test/build/

# make a git commit
commit:
	git add include/* src/* test/include/* test/src/* docs/* Makefile TODO.txt README.md
	git commit
