/* Generated by re2c 1.3 on Mon Mar 15 15:28:00 2021 */
#line 1 "Lexer.re"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "Utilities.h"
#include "PinkError.h"
#include "Token.h"
#include "Location.h"
#include "Lexer.h"


Lexer* CreateLexer()
{
  Lexer* lxr = (Lexer*)calloc(1, sizeof(Lexer));
  lxr->buf      = NULL;
  lxr->yych     = 0;
  lxr->end      = lxr->cursor = lxr->token = lxr->marker = lxr->buf;
  lxr->yyaccept = 0;
  lxr->state    = -1;
  return lxr;
}

void DestroyLexer(Lexer* lexer)
{
    free(lexer->buf);
    free(lexer);
}


void yySetBuffer(Lexer* lexer, char* text, int len)
{
    if (lexer->buf != NULL)
        free(lexer->buf);
    lexer->buf    = (char*)calloc(sizeof(char), len);
    lexer->buf    = strncpy(lexer->buf, text, len);
    lexer->cursor = lexer->marker = lexer->mrkctx = lexer->token = lexer->buf;
    lexer->end    = lexer->buf + len;
}


char* yyText(Lexer* lexer)
{
    return dupnstr(lexer->token, lexer->cursor - lexer->token);
}

Location* yyLloc(Lexer* lexer)
{
    return &(lexer->yylloc);
}

void updatelloc(Location* llocp, char* token, int len)
{
    llocp->first_line   = llocp->last_line;
    llocp->first_column = llocp->last_column;

    for (int i = 0; i < len; i++)
    {
        if (token[i] == '\n')
        {
            llocp->last_line++;
            llocp->last_column = 1;
        }
        else
            llocp->last_column++;
    }
}


#line 74 "Lexer.re"


#define YYPEEK() *lexer->cursor
#define YYSKIP() ++(lexer->cursor)
#define YYBACKUP() lexer->marker = lexer->cursor
#define YYRESTORE() lexer->cursor = lexer->marker
#define YYBACKUPCTX() lexer->mrkctx = lexer->cursor
#define YYRESTORECTX() lexer->cursor = lexer->mrkctx
#define YYLESSTHAN(n) (lexer->end - lexer->cursor) < n

int yyLex(Lexer* lexer)
{
    LOOP:
    lexer->token = lexer->cursor;
    
#line 89 "Lexer.c"
{
	char yych;
	yych = YYPEEK ();
	switch (yych) {
	case '\t':
	case '\n':
	case ' ':	goto yy4;
	case '!':
	case '$':
	case '%':
	case '&':
	case '*':
	case '+':
	case '/':
	case '>':
	case '@':
	case '^':
	case '|':
	case '~':	goto yy6;
	case '(':	goto yy9;
	case ')':	goto yy11;
	case '-':	goto yy13;
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':	goto yy14;
	case ':':	goto yy17;
	case ';':	goto yy19;
	case '<':	goto yy21;
	case '=':	goto yy22;
	case 'A':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case 'G':
	case 'H':
	case 'J':
	case 'K':
	case 'L':
	case 'M':
	case 'O':
	case 'P':
	case 'Q':
	case 'R':
	case 'S':
	case 'T':
	case 'U':
	case 'V':
	case 'W':
	case 'X':
	case 'Y':
	case 'Z':
	case '_':
	case 'a':
	case 'b':
	case 'c':
	case 'g':
	case 'h':
	case 'j':
	case 'k':
	case 'l':
	case 'm':
	case 'o':
	case 'p':
	case 'q':
	case 'r':
	case 's':
	case 'u':
	case 'v':
	case 'x':
	case 'y':
	case 'z':	goto yy23;
	case 'B':	goto yy26;
	case 'I':	goto yy27;
	case 'N':	goto yy28;
	case '\\':	goto yy29;
	case 'd':	goto yy31;
	case 'e':	goto yy32;
	case 'f':	goto yy33;
	case 'i':	goto yy34;
	case 'n':	goto yy35;
	case 't':	goto yy36;
	case 'w':	goto yy37;
	default:
		if (YYLESSTHAN (1)) {
			goto yyeof;
		}
		goto yy2;
	}
yy2:
	YYSKIP ();
yy3:
#line 93 "Lexer.re"
	{ updatelloc(&(lexer->yylloc), lexer->token, lexer->cursor - lexer->token); return T_ERR; }
#line 191 "Lexer.c"
yy4:
	YYSKIP ();
#line 95 "Lexer.re"
	{ updatelloc(&(lexer->yylloc), lexer->token, lexer->cursor - lexer->token); goto LOOP; }
#line 196 "Lexer.c"
yy6:
	YYSKIP ();
	yych = YYPEEK ();
yy7:
	switch (yych) {
	case '!':
	case '$':
	case '%':
	case '&':
	case '*':
	case '+':
	case '-':
	case '/':
	case ':':
	case '<':
	case '=':
	case '>':
	case '@':
	case '^':
	case '|':
	case '~':	goto yy6;
	default:
		if (YYLESSTHAN (1)) {
		}
		goto yy8;
	}
yy8:
#line 118 "Lexer.re"
	{ updatelloc(&(lexer->yylloc), lexer->token, lexer->cursor - lexer->token); return T_OPERATOR; }
#line 226 "Lexer.c"
yy9:
	YYSKIP ();
#line 109 "Lexer.re"
	{ updatelloc(&(lexer->yylloc), lexer->token, lexer->cursor - lexer->token); return T_LPAREN; }
#line 231 "Lexer.c"
yy11:
	YYSKIP ();
#line 110 "Lexer.re"
	{ updatelloc(&(lexer->yylloc), lexer->token, lexer->cursor - lexer->token); return T_RPAREN; }
#line 236 "Lexer.c"
yy13:
	YYSKIP ();
	yych = YYPEEK ();
	switch (yych) {
	case 0x00:
		if (YYLESSTHAN (1)) {
		}
		goto yy8;
	case '>':	goto yy38;
	default:	goto yy7;
	}
yy14:
	YYSKIP ();
	yych = YYPEEK ();
	switch (yych) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':	goto yy14;
	default:
		if (YYLESSTHAN (1)) {
		}
		goto yy16;
	}
yy16:
#line 101 "Lexer.re"
	{ updatelloc(&(lexer->yylloc), lexer->token, lexer->cursor - lexer->token); return T_INTEGER; }
#line 270 "Lexer.c"
yy17:
	YYSKIP ();
	yych = YYPEEK ();
	switch (yych) {
	case '!':
	case '$':
	case '%':
	case '&':
	case '*':
	case '+':
	case '-':
	case '/':
	case ':':
	case '<':
	case '>':
	case '@':
	case '^':
	case '|':
	case '~':	goto yy6;
	case '=':	goto yy40;
	default:
		if (YYLESSTHAN (1)) {
		}
		goto yy18;
	}
yy18:
#line 104 "Lexer.re"
	{ updatelloc(&(lexer->yylloc), lexer->token, lexer->cursor - lexer->token); return T_COLON; }
#line 299 "Lexer.c"
yy19:
	YYSKIP ();
#line 111 "Lexer.re"
	{ updatelloc(&(lexer->yylloc), lexer->token, lexer->cursor - lexer->token); return T_SEMICOLON; }
#line 304 "Lexer.c"
yy21:
	YYSKIP ();
	yych = YYPEEK ();
	switch (yych) {
	case 0x00:
		if (YYLESSTHAN (1)) {
		}
		goto yy8;
	case '-':	goto yy42;
	default:	goto yy7;
	}
yy22:
	YYSKIP ();
	yych = YYPEEK ();
	switch (yych) {
	case 0x00:
		if (YYLESSTHAN (1)) {
		}
		goto yy8;
	case '>':	goto yy44;
	default:	goto yy7;
	}
yy23:
	YYSKIP ();
	yych = YYPEEK ();
yy24:
	switch (yych) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case 'G':
	case 'H':
	case 'I':
	case 'J':
	case 'K':
	case 'L':
	case 'M':
	case 'N':
	case 'O':
	case 'P':
	case 'Q':
	case 'R':
	case 'S':
	case 'T':
	case 'U':
	case 'V':
	case 'W':
	case 'X':
	case 'Y':
	case 'Z':
	case '_':
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
	case 'g':
	case 'h':
	case 'i':
	case 'j':
	case 'k':
	case 'l':
	case 'm':
	case 'n':
	case 'o':
	case 'p':
	case 'q':
	case 'r':
	case 's':
	case 't':
	case 'u':
	case 'v':
	case 'w':
	case 'x':
	case 'y':
	case 'z':	goto yy23;
	default:
		if (YYLESSTHAN (1)) {
		}
		goto yy25;
	}
yy25:
#line 119 "Lexer.re"
	{ updatelloc(&(lexer->yylloc), lexer->token, lexer->cursor - lexer->token); return T_IDENTIFIER; }
#line 403 "Lexer.c"
yy26:
	YYSKIP ();
	yych = YYPEEK ();
	switch (yych) {
	case 0x00:
		if (YYLESSTHAN (1)) {
		}
		goto yy25;
	case 'o':	goto yy46;
	default:	goto yy24;
	}
yy27:
	YYSKIP ();
	yych = YYPEEK ();
	switch (yych) {
	case 0x00:
		if (YYLESSTHAN (1)) {
		}
		goto yy25;
	case 'n':	goto yy47;
	default:	goto yy24;
	}
yy28:
	YYSKIP ();
	yych = YYPEEK ();
	switch (yych) {
	case 0x00:
		if (YYLESSTHAN (1)) {
		}
		goto yy25;
	case 'i':	goto yy48;
	default:	goto yy24;
	}
yy29:
	YYSKIP ();
#line 103 "Lexer.re"
	{ updatelloc(&(lexer->yylloc), lexer->token, lexer->cursor - lexer->token); return T_BSLASH; }
#line 441 "Lexer.c"
yy31:
	YYSKIP ();
	yych = YYPEEK ();
	switch (yych) {
	case 0x00:
		if (YYLESSTHAN (1)) {
		}
		goto yy25;
	case 'o':	goto yy49;
	default:	goto yy24;
	}
yy32:
	YYSKIP ();
	yych = YYPEEK ();
	switch (yych) {
	case 0x00:
		if (YYLESSTHAN (1)) {
		}
		goto yy25;
	case 'l':	goto yy51;
	case 'n':	goto yy52;
	default:	goto yy24;
	}
yy33:
	YYSKIP ();
	yych = YYPEEK ();
	switch (yych) {
	case 0x00:
		if (YYLESSTHAN (1)) {
		}
		goto yy25;
	case 'a':	goto yy53;
	default:	goto yy24;
	}
yy34:
	YYSKIP ();
	yych = YYPEEK ();
	switch (yych) {
	case 0x00:
		if (YYLESSTHAN (1)) {
		}
		goto yy25;
	case 'f':	goto yy54;
	default:	goto yy24;
	}
yy35:
	YYSKIP ();
	yych = YYPEEK ();
	switch (yych) {
	case 0x00:
		if (YYLESSTHAN (1)) {
		}
		goto yy25;
	case 'i':	goto yy56;
	default:	goto yy24;
	}
yy36:
	YYSKIP ();
	yych = YYPEEK ();
	switch (yych) {
	case 0x00:
		if (YYLESSTHAN (1)) {
		}
		goto yy25;
	case 'h':	goto yy57;
	case 'r':	goto yy58;
	default:	goto yy24;
	}
yy37:
	YYSKIP ();
	yych = YYPEEK ();
	switch (yych) {
	case 0x00:
		if (YYLESSTHAN (1)) {
		}
		goto yy25;
	case 'h':	goto yy59;
	default:	goto yy24;
	}
yy38:
	YYSKIP ();
	yych = YYPEEK ();
	switch (yych) {
	case '!':
	case '$':
	case '%':
	case '&':
	case '*':
	case '+':
	case '-':
	case '/':
	case ':':
	case '<':
	case '=':
	case '>':
	case '@':
	case '^':
	case '|':
	case '~':	goto yy6;
	default:
		if (YYLESSTHAN (1)) {
		}
		goto yy39;
	}
yy39:
#line 106 "Lexer.re"
	{ updatelloc(&(lexer->yylloc), lexer->token, lexer->cursor - lexer->token); return T_RARROW; }
#line 549 "Lexer.c"
yy40:
	YYSKIP ();
	yych = YYPEEK ();
	switch (yych) {
	case '!':
	case '$':
	case '%':
	case '&':
	case '*':
	case '+':
	case '-':
	case '/':
	case ':':
	case '<':
	case '=':
	case '>':
	case '@':
	case '^':
	case '|':
	case '~':	goto yy6;
	default:
		if (YYLESSTHAN (1)) {
		}
		goto yy41;
	}
yy41:
#line 108 "Lexer.re"
	{ updatelloc(&(lexer->yylloc), lexer->token, lexer->cursor - lexer->token); return T_COLONEQ; }
#line 578 "Lexer.c"
yy42:
	YYSKIP ();
	yych = YYPEEK ();
	switch (yych) {
	case '!':
	case '$':
	case '%':
	case '&':
	case '*':
	case '+':
	case '-':
	case '/':
	case ':':
	case '<':
	case '=':
	case '>':
	case '@':
	case '^':
	case '|':
	case '~':	goto yy6;
	default:
		if (YYLESSTHAN (1)) {
		}
		goto yy43;
	}
yy43:
#line 107 "Lexer.re"
	{ updatelloc(&(lexer->yylloc), lexer->token, lexer->cursor - lexer->token); return T_LARROW; }
#line 607 "Lexer.c"
yy44:
	YYSKIP ();
	yych = YYPEEK ();
	switch (yych) {
	case '!':
	case '$':
	case '%':
	case '&':
	case '*':
	case '+':
	case '-':
	case '/':
	case ':':
	case '<':
	case '=':
	case '>':
	case '@':
	case '^':
	case '|':
	case '~':	goto yy6;
	default:
		if (YYLESSTHAN (1)) {
		}
		goto yy45;
	}
yy45:
#line 105 "Lexer.re"
	{ updatelloc(&(lexer->yylloc), lexer->token, lexer->cursor - lexer->token); return T_EQRARROW; }
#line 636 "Lexer.c"
yy46:
	YYSKIP ();
	yych = YYPEEK ();
	switch (yych) {
	case 0x00:
		if (YYLESSTHAN (1)) {
		}
		goto yy25;
	case 'o':	goto yy60;
	default:	goto yy24;
	}
yy47:
	YYSKIP ();
	yych = YYPEEK ();
	switch (yych) {
	case 0x00:
		if (YYLESSTHAN (1)) {
		}
		goto yy25;
	case 't':	goto yy61;
	default:	goto yy24;
	}
yy48:
	YYSKIP ();
	yych = YYPEEK ();
	switch (yych) {
	case 0x00:
		if (YYLESSTHAN (1)) {
		}
		goto yy25;
	case 'l':	goto yy63;
	default:	goto yy24;
	}
yy49:
	YYSKIP ();
	yych = YYPEEK ();
	switch (yych) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case 'G':
	case 'H':
	case 'I':
	case 'J':
	case 'K':
	case 'L':
	case 'M':
	case 'N':
	case 'O':
	case 'P':
	case 'Q':
	case 'R':
	case 'S':
	case 'T':
	case 'U':
	case 'V':
	case 'W':
	case 'X':
	case 'Y':
	case 'Z':
	case '_':
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
	case 'g':
	case 'h':
	case 'i':
	case 'j':
	case 'k':
	case 'l':
	case 'm':
	case 'n':
	case 'o':
	case 'p':
	case 'q':
	case 'r':
	case 's':
	case 't':
	case 'u':
	case 'v':
	case 'w':
	case 'x':
	case 'y':
	case 'z':	goto yy23;
	default:
		if (YYLESSTHAN (1)) {
		}
		goto yy50;
	}
yy50:
#line 116 "Lexer.re"
	{ updatelloc(&(lexer->yylloc), lexer->token, lexer->cursor - lexer->token); return T_DO; }
#line 745 "Lexer.c"
yy51:
	YYSKIP ();
	yych = YYPEEK ();
	switch (yych) {
	case 0x00:
		if (YYLESSTHAN (1)) {
		}
		goto yy25;
	case 's':	goto yy65;
	default:	goto yy24;
	}
yy52:
	YYSKIP ();
	yych = YYPEEK ();
	switch (yych) {
	case 0x00:
		if (YYLESSTHAN (1)) {
		}
		goto yy25;
	case 'd':	goto yy66;
	default:	goto yy24;
	}
yy53:
	YYSKIP ();
	yych = YYPEEK ();
	switch (yych) {
	case 0x00:
		if (YYLESSTHAN (1)) {
		}
		goto yy25;
	case 'l':	goto yy68;
	default:	goto yy24;
	}
yy54:
	YYSKIP ();
	yych = YYPEEK ();
	switch (yych) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case 'G':
	case 'H':
	case 'I':
	case 'J':
	case 'K':
	case 'L':
	case 'M':
	case 'N':
	case 'O':
	case 'P':
	case 'Q':
	case 'R':
	case 'S':
	case 'T':
	case 'U':
	case 'V':
	case 'W':
	case 'X':
	case 'Y':
	case 'Z':
	case '_':
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
	case 'g':
	case 'h':
	case 'i':
	case 'j':
	case 'k':
	case 'l':
	case 'm':
	case 'n':
	case 'o':
	case 'p':
	case 'q':
	case 'r':
	case 's':
	case 't':
	case 'u':
	case 'v':
	case 'w':
	case 'x':
	case 'y':
	case 'z':	goto yy23;
	default:
		if (YYLESSTHAN (1)) {
		}
		goto yy55;
	}
yy55:
#line 112 "Lexer.re"
	{ updatelloc(&(lexer->yylloc), lexer->token, lexer->cursor - lexer->token); return T_IF; }
#line 854 "Lexer.c"
yy56:
	YYSKIP ();
	yych = YYPEEK ();
	switch (yych) {
	case 0x00:
		if (YYLESSTHAN (1)) {
		}
		goto yy25;
	case 'l':	goto yy69;
	default:	goto yy24;
	}
yy57:
	YYSKIP ();
	yych = YYPEEK ();
	switch (yych) {
	case 0x00:
		if (YYLESSTHAN (1)) {
		}
		goto yy25;
	case 'e':	goto yy71;
	default:	goto yy24;
	}
yy58:
	YYSKIP ();
	yych = YYPEEK ();
	switch (yych) {
	case 0x00:
		if (YYLESSTHAN (1)) {
		}
		goto yy25;
	case 'u':	goto yy72;
	default:	goto yy24;
	}
yy59:
	YYSKIP ();
	yych = YYPEEK ();
	switch (yych) {
	case 0x00:
		if (YYLESSTHAN (1)) {
		}
		goto yy25;
	case 'i':	goto yy73;
	default:	goto yy24;
	}
yy60:
	YYSKIP ();
	yych = YYPEEK ();
	switch (yych) {
	case 0x00:
		if (YYLESSTHAN (1)) {
		}
		goto yy25;
	case 'l':	goto yy74;
	default:	goto yy24;
	}
yy61:
	YYSKIP ();
	yych = YYPEEK ();
	switch (yych) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case 'G':
	case 'H':
	case 'I':
	case 'J':
	case 'K':
	case 'L':
	case 'M':
	case 'N':
	case 'O':
	case 'P':
	case 'Q':
	case 'R':
	case 'S':
	case 'T':
	case 'U':
	case 'V':
	case 'W':
	case 'X':
	case 'Y':
	case 'Z':
	case '_':
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
	case 'g':
	case 'h':
	case 'i':
	case 'j':
	case 'k':
	case 'l':
	case 'm':
	case 'n':
	case 'o':
	case 'p':
	case 'q':
	case 'r':
	case 's':
	case 't':
	case 'u':
	case 'v':
	case 'w':
	case 'x':
	case 'y':
	case 'z':	goto yy23;
	default:
		if (YYLESSTHAN (1)) {
		}
		goto yy62;
	}
yy62:
#line 102 "Lexer.re"
	{ updatelloc(&(lexer->yylloc), lexer->token, lexer->cursor - lexer->token); return T_TYPE_INT; }
#line 985 "Lexer.c"
yy63:
	YYSKIP ();
	yych = YYPEEK ();
	switch (yych) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case 'G':
	case 'H':
	case 'I':
	case 'J':
	case 'K':
	case 'L':
	case 'M':
	case 'N':
	case 'O':
	case 'P':
	case 'Q':
	case 'R':
	case 'S':
	case 'T':
	case 'U':
	case 'V':
	case 'W':
	case 'X':
	case 'Y':
	case 'Z':
	case '_':
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
	case 'g':
	case 'h':
	case 'i':
	case 'j':
	case 'k':
	case 'l':
	case 'm':
	case 'n':
	case 'o':
	case 'p':
	case 'q':
	case 'r':
	case 's':
	case 't':
	case 'u':
	case 'v':
	case 'w':
	case 'x':
	case 'y':
	case 'z':	goto yy23;
	default:
		if (YYLESSTHAN (1)) {
		}
		goto yy64;
	}
yy64:
#line 97 "Lexer.re"
	{ updatelloc(&(lexer->yylloc), lexer->token, lexer->cursor - lexer->token); return T_TYPE_NIL; }
#line 1061 "Lexer.c"
yy65:
	YYSKIP ();
	yych = YYPEEK ();
	switch (yych) {
	case 0x00:
		if (YYLESSTHAN (1)) {
		}
		goto yy25;
	case 'e':	goto yy76;
	default:	goto yy24;
	}
yy66:
	YYSKIP ();
	yych = YYPEEK ();
	switch (yych) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case 'G':
	case 'H':
	case 'I':
	case 'J':
	case 'K':
	case 'L':
	case 'M':
	case 'N':
	case 'O':
	case 'P':
	case 'Q':
	case 'R':
	case 'S':
	case 'T':
	case 'U':
	case 'V':
	case 'W':
	case 'X':
	case 'Y':
	case 'Z':
	case '_':
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
	case 'g':
	case 'h':
	case 'i':
	case 'j':
	case 'k':
	case 'l':
	case 'm':
	case 'n':
	case 'o':
	case 'p':
	case 'q':
	case 'r':
	case 's':
	case 't':
	case 'u':
	case 'v':
	case 'w':
	case 'x':
	case 'y':
	case 'z':	goto yy23;
	default:
		if (YYLESSTHAN (1)) {
		}
		goto yy67;
	}
yy67:
#line 117 "Lexer.re"
	{ updatelloc(&(lexer->yylloc), lexer->token, lexer->cursor - lexer->token); return T_END; }
#line 1148 "Lexer.c"
yy68:
	YYSKIP ();
	yych = YYPEEK ();
	switch (yych) {
	case 0x00:
		if (YYLESSTHAN (1)) {
		}
		goto yy25;
	case 's':	goto yy78;
	default:	goto yy24;
	}
yy69:
	YYSKIP ();
	yych = YYPEEK ();
	switch (yych) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case 'G':
	case 'H':
	case 'I':
	case 'J':
	case 'K':
	case 'L':
	case 'M':
	case 'N':
	case 'O':
	case 'P':
	case 'Q':
	case 'R':
	case 'S':
	case 'T':
	case 'U':
	case 'V':
	case 'W':
	case 'X':
	case 'Y':
	case 'Z':
	case '_':
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
	case 'g':
	case 'h':
	case 'i':
	case 'j':
	case 'k':
	case 'l':
	case 'm':
	case 'n':
	case 'o':
	case 'p':
	case 'q':
	case 'r':
	case 's':
	case 't':
	case 'u':
	case 'v':
	case 'w':
	case 'x':
	case 'y':
	case 'z':	goto yy23;
	default:
		if (YYLESSTHAN (1)) {
		}
		goto yy70;
	}
yy70:
#line 96 "Lexer.re"
	{ updatelloc(&(lexer->yylloc), lexer->token, lexer->cursor - lexer->token); return T_NIL; }
#line 1235 "Lexer.c"
yy71:
	YYSKIP ();
	yych = YYPEEK ();
	switch (yych) {
	case 0x00:
		if (YYLESSTHAN (1)) {
		}
		goto yy25;
	case 'n':	goto yy79;
	default:	goto yy24;
	}
yy72:
	YYSKIP ();
	yych = YYPEEK ();
	switch (yych) {
	case 0x00:
		if (YYLESSTHAN (1)) {
		}
		goto yy25;
	case 'e':	goto yy81;
	default:	goto yy24;
	}
yy73:
	YYSKIP ();
	yych = YYPEEK ();
	switch (yych) {
	case 0x00:
		if (YYLESSTHAN (1)) {
		}
		goto yy25;
	case 'l':	goto yy83;
	default:	goto yy24;
	}
yy74:
	YYSKIP ();
	yych = YYPEEK ();
	switch (yych) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case 'G':
	case 'H':
	case 'I':
	case 'J':
	case 'K':
	case 'L':
	case 'M':
	case 'N':
	case 'O':
	case 'P':
	case 'Q':
	case 'R':
	case 'S':
	case 'T':
	case 'U':
	case 'V':
	case 'W':
	case 'X':
	case 'Y':
	case 'Z':
	case '_':
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
	case 'g':
	case 'h':
	case 'i':
	case 'j':
	case 'k':
	case 'l':
	case 'm':
	case 'n':
	case 'o':
	case 'p':
	case 'q':
	case 'r':
	case 's':
	case 't':
	case 'u':
	case 'v':
	case 'w':
	case 'x':
	case 'y':
	case 'z':	goto yy23;
	default:
		if (YYLESSTHAN (1)) {
		}
		goto yy75;
	}
yy75:
#line 100 "Lexer.re"
	{ updatelloc(&(lexer->yylloc), lexer->token, lexer->cursor - lexer->token); return T_TYPE_BOOL; }
#line 1344 "Lexer.c"
yy76:
	YYSKIP ();
	yych = YYPEEK ();
	switch (yych) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case 'G':
	case 'H':
	case 'I':
	case 'J':
	case 'K':
	case 'L':
	case 'M':
	case 'N':
	case 'O':
	case 'P':
	case 'Q':
	case 'R':
	case 'S':
	case 'T':
	case 'U':
	case 'V':
	case 'W':
	case 'X':
	case 'Y':
	case 'Z':
	case '_':
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
	case 'g':
	case 'h':
	case 'i':
	case 'j':
	case 'k':
	case 'l':
	case 'm':
	case 'n':
	case 'o':
	case 'p':
	case 'q':
	case 'r':
	case 's':
	case 't':
	case 'u':
	case 'v':
	case 'w':
	case 'x':
	case 'y':
	case 'z':	goto yy23;
	default:
		if (YYLESSTHAN (1)) {
		}
		goto yy77;
	}
yy77:
#line 114 "Lexer.re"
	{ updatelloc(&(lexer->yylloc), lexer->token, lexer->cursor - lexer->token); return T_ELSE; }
#line 1420 "Lexer.c"
yy78:
	YYSKIP ();
	yych = YYPEEK ();
	switch (yych) {
	case 0x00:
		if (YYLESSTHAN (1)) {
		}
		goto yy25;
	case 'e':	goto yy84;
	default:	goto yy24;
	}
yy79:
	YYSKIP ();
	yych = YYPEEK ();
	switch (yych) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case 'G':
	case 'H':
	case 'I':
	case 'J':
	case 'K':
	case 'L':
	case 'M':
	case 'N':
	case 'O':
	case 'P':
	case 'Q':
	case 'R':
	case 'S':
	case 'T':
	case 'U':
	case 'V':
	case 'W':
	case 'X':
	case 'Y':
	case 'Z':
	case '_':
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
	case 'g':
	case 'h':
	case 'i':
	case 'j':
	case 'k':
	case 'l':
	case 'm':
	case 'n':
	case 'o':
	case 'p':
	case 'q':
	case 'r':
	case 's':
	case 't':
	case 'u':
	case 'v':
	case 'w':
	case 'x':
	case 'y':
	case 'z':	goto yy23;
	default:
		if (YYLESSTHAN (1)) {
		}
		goto yy80;
	}
yy80:
#line 113 "Lexer.re"
	{ updatelloc(&(lexer->yylloc), lexer->token, lexer->cursor - lexer->token); return T_THEN; }
#line 1507 "Lexer.c"
yy81:
	YYSKIP ();
	yych = YYPEEK ();
	switch (yych) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case 'G':
	case 'H':
	case 'I':
	case 'J':
	case 'K':
	case 'L':
	case 'M':
	case 'N':
	case 'O':
	case 'P':
	case 'Q':
	case 'R':
	case 'S':
	case 'T':
	case 'U':
	case 'V':
	case 'W':
	case 'X':
	case 'Y':
	case 'Z':
	case '_':
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
	case 'g':
	case 'h':
	case 'i':
	case 'j':
	case 'k':
	case 'l':
	case 'm':
	case 'n':
	case 'o':
	case 'p':
	case 'q':
	case 'r':
	case 's':
	case 't':
	case 'u':
	case 'v':
	case 'w':
	case 'x':
	case 'y':
	case 'z':	goto yy23;
	default:
		if (YYLESSTHAN (1)) {
		}
		goto yy82;
	}
yy82:
#line 98 "Lexer.re"
	{ updatelloc(&(lexer->yylloc), lexer->token, lexer->cursor - lexer->token); return T_TRUE; }
#line 1583 "Lexer.c"
yy83:
	YYSKIP ();
	yych = YYPEEK ();
	switch (yych) {
	case 0x00:
		if (YYLESSTHAN (1)) {
		}
		goto yy25;
	case 'e':	goto yy86;
	default:	goto yy24;
	}
yy84:
	YYSKIP ();
	yych = YYPEEK ();
	switch (yych) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case 'G':
	case 'H':
	case 'I':
	case 'J':
	case 'K':
	case 'L':
	case 'M':
	case 'N':
	case 'O':
	case 'P':
	case 'Q':
	case 'R':
	case 'S':
	case 'T':
	case 'U':
	case 'V':
	case 'W':
	case 'X':
	case 'Y':
	case 'Z':
	case '_':
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
	case 'g':
	case 'h':
	case 'i':
	case 'j':
	case 'k':
	case 'l':
	case 'm':
	case 'n':
	case 'o':
	case 'p':
	case 'q':
	case 'r':
	case 's':
	case 't':
	case 'u':
	case 'v':
	case 'w':
	case 'x':
	case 'y':
	case 'z':	goto yy23;
	default:
		if (YYLESSTHAN (1)) {
		}
		goto yy85;
	}
yy85:
#line 99 "Lexer.re"
	{ updatelloc(&(lexer->yylloc), lexer->token, lexer->cursor - lexer->token); return T_FALSE; }
#line 1670 "Lexer.c"
yy86:
	YYSKIP ();
	yych = YYPEEK ();
	switch (yych) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case 'G':
	case 'H':
	case 'I':
	case 'J':
	case 'K':
	case 'L':
	case 'M':
	case 'N':
	case 'O':
	case 'P':
	case 'Q':
	case 'R':
	case 'S':
	case 'T':
	case 'U':
	case 'V':
	case 'W':
	case 'X':
	case 'Y':
	case 'Z':
	case '_':
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
	case 'g':
	case 'h':
	case 'i':
	case 'j':
	case 'k':
	case 'l':
	case 'm':
	case 'n':
	case 'o':
	case 'p':
	case 'q':
	case 'r':
	case 's':
	case 't':
	case 'u':
	case 'v':
	case 'w':
	case 'x':
	case 'y':
	case 'z':	goto yy23;
	default:
		if (YYLESSTHAN (1)) {
		}
		goto yy87;
	}
yy87:
#line 115 "Lexer.re"
	{ updatelloc(&(lexer->yylloc), lexer->token, lexer->cursor - lexer->token); return T_WHILE; }
#line 1746 "Lexer.c"
yyeof:
#line 94 "Lexer.re"
	{ updatelloc(&(lexer->yylloc), lexer->token, lexer->cursor - lexer->token); return T_EOF; }
#line 1750 "Lexer.c"
}
#line 121 "Lexer.re"

}
