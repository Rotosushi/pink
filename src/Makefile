CC = clang
CARGS = -g -std=c11

LINK_STEP = Application.o Assignment.o Conditional.o Iteration.o \
StringInterner.o Variable.o Bind.o Binop.o Unop.o BinopEliminators.o \
UnopEliminators.o BinopPrecedence.o BinopTable.o UnopTable.o Nil.o \
Integer.o Boolean.o Lambda.o ScalarType.o ProcType.o Type.o TypeLiteral.o Object.o \
Ast.o TypeInterner.o Environment.o SymbolTable.o PinkError.o Token.o \
Lexer.o Parser.o PinkKernel.o Repl.o Utilities.o Pink.o -o pink


pink: Types Objects DataStructures SymbolicComputation Frontend Aux Main
	${CC} ${CARGS} ${LINK_STEP}

clean:
	rm -f *.o pink

debug: pink
	gdb -tui pink

DataStructures: StringInterner TypeInterner SymbolTable BinopEliminators BinopTable BinopPrecedence UnopEliminators UnopTable Environment

StringInterner:
	${CC} ${CARGS} -c StringInterner.c -o StringInterner.o

TypeInterner:
	${CC} ${CARGS} -c TypeInterner.c -o TypeInterner.o

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
	${CC} ${CARGS} -c Environment.c -o Environment.o

SymbolicComputation: Application Variable Bind Assignment Conditional Iteration Binop Unop Ast

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

Ast:
	${CC} ${CARGS} -c Ast.c -o Ast.o

Types: Type ScalarType ProcType TypeLiteral

Type:
	${CC} ${CARGS} -c Type.c -o Type.o

ScalarType:
	${CC} ${CARGS} -c ScalarType.c -o ScalarType.o

ProcType:
	${CC} ${CARGS} -c ProcType.c -o ProcType.o

TypeLiteral:
	${CC} ${CARGS} -c TypeLiteral.c -o TypeLiteral.o

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

Aux: PinkError PinkKernel Repl Utils

PinkError:
	${CC} ${CARGS} -c PinkError.c -o PinkError.o

PinkKernel:
	${CC} ${CARGS} -c PinkKernel.c -o PinkKernel.o

Repl:
	${CC} ${CARGS} -c Repl.c -o Repl.o

Utils:
	${CC} ${CARGS} -c Utilities.c -o Utilities.o

Main:
	${CC} ${CARGS} -c Pink.c -o Pink.o
