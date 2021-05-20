CC = clang++
COMPILER_ARGS = -g -std=c++17 -I${HOME}/Development/Source/llvm-12/include
LINKER_ARGS   = -g -std=c++17 -L${HOME}/Development/Source/llvm-12/lib

OBJECT_FILES  = Location.o Judgement.o ScopeSet.o StringInterner.o \
PinkError.o \
Ast.o \
Object.o Nil.o Boolean.o Integer.o TypeLiteral.o ValueLiteral.o


Pink: SymbolicComputation Objects DataStructures Frontend Aux Main
	${CC} ${LINKER_ARGS} ${OBJECT_FILES}

clean:
	rm -f *.o Pink

debug: Pink
	gdb -tui Pink

Objects: Object Nil Boolean Integer TypeLiteral ValueLiteral

Object:
	${CC} ${COMPILER_ARGS} -c Object.cpp -o Object.o

Nil:
	${CC} ${COMPILER_ARGS} -c Nil.cpp -o Nil.o

Boolean:
	${CC} ${COMPILER_ARGS} -c Boolean.cpp -o Boolean.o

Integer:
	${CC} ${COMPILER_ARGS} -c Integer.cpp -o Integer.o

TypeLiteral:
	${CC} ${COMPILER_ARGS} -c TypeLiteral.cpp -o TypeLiteral.o

ValueLiteral:
	${CC} ${COMPILER_ARGS} -c ValueLiteral.cpp -o ValueLiteral.o



SymbolicComputation: Ast Bind Application Binop Unop

Ast:
	${CC} ${COMPILER_ARGS} -c Ast.cpp -o Ast.o



DataStructures: Location Judgement StringInterner ScopeSet SymbolTable \
BinopLiteralTable BinopTable UnopLiteralTable UnopTable Argument \
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

BinopTable:
	${CC} ${COMPILER_ARGS} -c BinopTable.cpp -o -BinopTable.o

UnopLiteralTable:
	${CC} ${COMPILER_ARGS} -c UnopLiteralTable.cpp -o UnopLiteralTable.o

UnopTable:
	${CC} ${COMPIELR_ARGS} -c UnopTable.cpp -o UnopTable.o

Environment:
	${CC} ${COMPIELR_ARGS} -c Environment.cpp -o Environment.o


Fontend:




Aux: PinkError Environment

PinkError:
	${CC} ${COMPILER_ARGS} -c PinkError.cpp -o PinkError.o




Main:
