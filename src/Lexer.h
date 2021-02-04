#ifndef LEXER_H
#define LEXER_H

#include <stdlib.h>


#include "Location.h"


/*
	this is a buffered scanner,
	buf, is the buffer
	end is the end of the filled slots in the buffer.
	cursor is used to gather slots into single tokens.
	token is the begining of the current token being processed.
	yyaccept, yych, and state are used internally by re2c for
	keeping track of it's state to allow for reentrancy.

*/
typedef struct Lexer {
	char* buf;
  char* end;
  char* cursor;
	char* marker;
	char* mrkctx;
  char* token;
	unsigned int yyaccept;
	int state;
	char  yych;
	Location yylloc;
} Lexer;


Lexer*   CreateLexer();
void     DestroyLexer(Lexer* lexer);
void     ResetLexer(Lexer* lexer);
void     yySetBuffer(Lexer* lexer, char* text, int len);
char*    yyText(Lexer* lexer);
Location*  yyLloc(Lexer* lexer);
int      yyLex(Lexer* lexer);

#endif // !LEXER_H
