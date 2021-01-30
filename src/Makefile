CC = clang
CARGS = -fstandalone-debug -fexceptions -g -std=c11

pink: DataStructures SymbolicComputation Types Objects Frontend Utilities
	${CC} ${CARGS} -o pink StringInterner.o SymbolTable.o BinopEliminators.o
	BinopTable.o BinopPrecedence.o UnopEliminators.o UnopTable.o
	Application.o Variable.o Bind.o Assignment.o Conditional.o Iteration.o
	Binop.o Unop.o ScalarType.o ProcType.o Type.o Nil.o Integer.o
	Boolean.o Lambda.o Object.o PinkError.o

clean:
	rm -f *.o pink

debug: pink
	gdb -tui pink

DataStructures: StringInterner SymbolTable BinopEliminators BinopTable BinopPrecedence UnopEliminators UnopTable Environment

StringInterner:
	${CC} ${CARGS} -c StringInterner.c -o StringInterner.o

SymbolTable:
	${CC} ${CARGS} -c SymbolTable.c -o SymbolTable.o

BinopEliminators:
	${CC} ${CARGS} -c BinopEliminators.c -o BinopEliminators.o

BinopTable:
	${CC} ${CARGS} -c BinopTable.c -o BinopTable.o

BinopPrecedence:
	${CC} ${CARGS} -c BinopPrecedence.c -o BinopPrecedence.o

UnopEliminators:
	${CC} ${CARGS} -c UnopEliminators.c -o UnopEliminators.o

UnopTable:
	${CC} ${CARGS} -c UnopTable.c -o UnopTable.o

Environment:
	${CC} ${CARGS} -c Environment.c -o Environemnt.o

SymbolicComputation: Application Variable Bind Assignment Conditional Iteration Binop Unop

Application:
	${CC} ${CARGS} -c Application.c -o Application.o

Variable:
	${CC} ${CARGS} -c Variable.c -o Variable.o

Bind:
	${CC} ${CARGS} -c Bind.c -o Bind.o

Assignment:
	${CC} ${CARGS} -c Assignment.c -o Assignment.o

Conditional:
	${CC} ${CARGS} -c Conditional.c -o Conditional.o

Iteration:
	${CC} ${CARGS} -c Iteration.c -o Iteration.o

Binop:
	${CC} ${CARGS} -c Binop.c -o Binop.o

Unop:
	${CC} ${CARGS} -c Unop.c -o Unop.o


Types: Type ScalarType ProcType

Type:
	${CC} ${CARGS} -c Type.c -o Type.o

ScalarType:
	${CC} ${CARGS} -c ScalarType.c -o ScalarType.o

ProcType:
	${CC} ${CARGS} -c ProcType.c -o ProcType.o


Objects: Nil Integer Boolean Lambda Object

Nil:
	${CC} ${CARGS} -c Nil.c -o Nil.o

Integer:
	${CC} ${CARGS} -c Integer.c -o Integer.o

Boolean:
	${CC} ${CARGS} -c Boolean.c -o Boolean.o

Lambda:
	${CC} ${CARGS} -c Lambda.c -o Lambda.o

Object:
	${CC} ${CARGS} -c Object.c -o Object.o


Frontend: Token Lexer Parser

Token:
	${CC} ${CARGS} -c Token.c -o Token.o

Lexer: re2c
	${CC} ${CARGS} -c Lexer.c -o Lexer.o

re2c: Lexer.re
	re2c -W --input custom Lexer.re -o Lexer.c

Parser:
	${CC} ${CARGS} -c Parser.c -o Parser.o

Utilities: PinkError

PinkError:
	${CC} ${CARGS} -c PinkError.c -o PinkError.o
