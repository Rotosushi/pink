CC = clang++
COMPILER_ARGS = -g -std=c++17 -I${HOME}/Development/Source/llvm-12/include
LINKER_ARGS   = -g -std=c++17 -L${HOME}/Development/Source/llvm-12/lib

OBJECT_FILES  = Location.o Judgement.o ScopeSet.o StringInterner.o \
PinkError.o \
Ast.o


Pink: Types Objects DataStructures SymbolicComputation Frontend Aux Main
	${CC} ${LINKER_ARGS} ${OBJECT_FILES}

clean:
	rm -f *.o Pink

debug: Pink
	gdb -tui Pink



Types:




Objects:





DataStructures: Location Judgement StringInterner ScopeSet SymbolTable

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



SymbolicComputation: Ast

Ast:
	${CC} ${COMPILER_ARGS} -c Ast.cpp -o Ast.o



Fontend:




Aux: PinkError

PinkError:
	${CC} ${COMPILER_ARGS} -c PinkError.cpp -o PinkError.o




Main:
