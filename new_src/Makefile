CC = clang++
COMPILER_ARGS = -g -std=c++17 -I${HOME}/Development/Source/llvm-12/include
LINKER_ARGS   = -g -std=c++17 -L${HOME}/Development/Source/llvm-12/lib

OBJECT_FILES  = Location.o Judgement.o ScopeSet.o StringInterner.o \
BinopTable.o BinopLiteralTable.o BinopPrecedenceTable.o UnopTable.o UnopLiteralTable.o \
SymbolTable.o Environment.o Error.o Kernel.o Gensym.o Lexer.o Parser.o\
Ast.o Variable.o Assignment.o Application.o Bind.o Binop.o Unop.o \
Nil.o Boolean.o Integer.o Pointer.o Lambda.o Closure.o TypeLiteral.o ValueLiteral.o


Pink: SymbolicComputation Objects DataStructures Frontend Aux Main
	${CC} ${LINKER_ARGS} ${OBJECT_FILES}

clean:
	rm -f *.o Pink

debug: Pink
	gdb -tui Pink

Objects: Nil Boolean Integer Lambda Closure TypeLiteral ValueLiteral

Nil:
	${CC} ${COMPILER_ARGS} -c Nil.cpp -o Nil.o

Boolean:
	${CC} ${COMPILER_ARGS} -c Boolean.cpp -o Boolean.o

Integer:
	${CC} ${COMPILER_ARGS} -c Integer.cpp -o Integer.o

Lambda:
	${CC} ${COMPILER_ARGS} -c Lambda.cpp -o Lambda.o

Closure:
	${CC} ${COMPILER_ARGS} -c Closure.cpp -o Closure.o

TypeLiteral:
	${CC} ${COMPILER_ARGS} -c TypeLiteral.cpp -o TypeLiteral.o

ValueLiteral:
	${CC} ${COMPILER_ARGS} -c ValueLiteral.cpp -o ValueLiteral.o



SymbolicComputation: Ast Variable Assignment Application Bind Binop Unop

Ast:
	${CC} ${COMPILER_ARGS} -c Ast.cpp -o Ast.o

Variable:
	${CC} ${COMPILER_ARGS} -c Variable.cpp -o Variable.o

Assignment:
	${CC} ${COMPILER_ARGS} -c Assignment.cpp -o Assignment.o

Application:
	${CC} ${COMPILER_ARGS} -c Application.cpp -o Application.o

Bind:
	${CC} ${COMPILER_ARGS} -c Bind.cpp -o Bind.o

Binop:
	${CC} ${COMPIELR_ARGS} -c Binop.cpp -o Binop.o

Unop:
	${CC} ${COMPILER_ARGS} -c Unop.cpp -o Unop.o


DataStructures: Location Judgement StringInterner ScopeSet SymbolTable \
BinopLiteralTable BinopPrecedenceTable BinopTable UnopLiteralTable UnopTable \
Environment

Location:
	${CC} ${COMPILER_ARGS} -c Location.cpp -o Location.o

Judgement:
	${CC} ${COMPILER_ARGS} -c Judgement.cpp -o Judgement.o

StringInterner:
	${CC} ${COMPILER_ARGS} -c StringInterner.cpp -o StringInterner.o

ScopeSet:
	${CC} ${COMPILER_ARGS} -c ScopeSet.cpp -o ScopeSet.o

SymbolTable:
	${CC} ${COMPILER_ARGS} -c SymbolTable.cpp -o SymbolTable.o

BinopLiteralTable:
	${CC} ${COMPILER_ARGS} -c BinopLiteralTable.cpp -o BinopLiteralTable.o

BinopPrecedenceTable:
	${CC} ${COMPILER_ARGS} -c BinopPrecedenceTable.cpp -o BinopPrecedenceTable.o

BinopTable:
	${CC} ${COMPILER_ARGS} -c BinopTable.cpp -o -BinopTable.o

UnopLiteralTable:
	${CC} ${COMPILER_ARGS} -c UnopLiteralTable.cpp -o UnopLiteralTable.o

UnopTable:
	${CC} ${COMPIELR_ARGS} -c UnopTable.cpp -o UnopTable.o

Environment:
	${CC} ${COMPIELR_ARGS} -c Environment.cpp -o Environment.o


Fontend: Lexer Parser

Lexer: re2c
	${CC} ${COMPILER_ARGS} -c Lexer.cpp -o Lexer.o

re2c:
	re2c -W --input custom Lexer.re -o Lexer.cpp

Parser:
	${CC} ${COMPILER_ARGS} -c Parser.cpp -o Parser.o

Aux: Error PinkKernel

Error:
	${CC} ${COMPILER_ARGS} -c Error.cpp -o Error.o

PinkKernel:
	${CC} ${COMPILER_ARGS} -c PinkKernel.cpp -o PinkKernel.o



Main:
