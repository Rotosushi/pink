#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>


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

// god why isn't strndup a stdlib procedure?
char* dupstr(char* str, int len)
{
    char* result = (char*)calloc(sizeof(char), len + 1);
    result = strncpy(result, str, len);
    return result;
}

char* yyText(Lexer* lexer)
{
    return dupstr(lexer->token, lexer->cursor - lexer->token);
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


/*!re2c
    identifier = [a-zA-Z_][a-zA-Z0-9_]*;
    integer    = [0-9]+;
    operator   = [+\-*/%<>=:&@!~|$^]+;
*/

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
    /*!re2c
        re2c:define:YYCTYPE = char;
        re2c:yyfill:enable = 0;
        re2c:eof = 0;

        *          { updatelloc(&(lexer->yylloc), lexer->token, lexer->cursor - lexer->token); return T_ERR; }
        $          { updatelloc(&(lexer->yylloc), lexer->token, lexer->cursor - lexer->token); return T_EOF; }
        [ \t\n]    { updatelloc(&(lexer->yylloc), lexer->token, lexer->cursor - lexer->token); goto LOOP; }
        "nil"      { updatelloc(&(lexer->yylloc), lexer->token, lexer->cursor - lexer->token); return T_NIL; }
        "Nil"      { updatelloc(&(lexer->yylloc), lexer->token, lexer->cursor - lexer->token); return T_TYPE_NIL; }
        "true"     { updatelloc(&(lexer->yylloc), lexer->token, lexer->cursor - lexer->token); return T_TRUE; }
        "false"    { updatelloc(&(lexer->yylloc), lexer->token, lexer->cursor - lexer->token); return T_FALSE; }
        "Bool"     { updatelloc(&(lexer->yylloc), lexer->token, lexer->cursor - lexer->token); return T_TYPE_BOOL; }
        integer    { updatelloc(&(lexer->yylloc), lexer->token, lexer->cursor - lexer->token); return T_INTEGER; }
        "Int"      { updatelloc(&(lexer->yylloc), lexer->token, lexer->cursor - lexer->token); return T_TYPE_INT; }
        "\\"       { updatelloc(&(lexer->yylloc), lexer->token, lexer->cursor - lexer->token); return T_BSLASH; }
        ":"        { updatelloc(&(lexer->yylloc), lexer->token, lexer->cursor - lexer->token); return T_COLON; }
        "=>"       { updatelloc(&(lexer->yylloc), lexer->token, lexer->cursor - lexer->token); return T_EQRARROW; }
        "->"       { updatelloc(&(lexer->yylloc), lexer->token, lexer->cursor - lexer->token); return T_RARROW; }
        "<-"       { updatelloc(&(lexer->yylloc), lexer->token, lexer->cursor - lexer->token); return T_LARROW; }
        ":="       { updatelloc(&(lexer->yylloc), lexer->token, lexer->cursor - lexer->token); return T_COLONEQ; }
        "("        { updatelloc(&(lexer->yylloc), lexer->token, lexer->cursor - lexer->token); return T_LPAREN; }
        ")"        { updatelloc(&(lexer->yylloc), lexer->token, lexer->cursor - lexer->token); return T_RPAREN; }
        ";"        { updatelloc(&(lexer->yylloc), lexer->token, lexer->cursor - lexer->token); return T_SEMICOLON; }
        "if"       { updatelloc(&(lexer->yylloc), lexer->token, lexer->cursor - lexer->token); return T_IF; }
        "then"     { updatelloc(&(lexer->yylloc), lexer->token, lexer->cursor - lexer->token); return T_THEN; }
        "else"     { updatelloc(&(lexer->yylloc), lexer->token, lexer->cursor - lexer->token); return T_ELSE; }
        "while"    { updatelloc(&(lexer->yylloc), lexer->token, lexer->cursor - lexer->token); return T_WHILE; }
        "do"       { updatelloc(&(lexer->yylloc), lexer->token, lexer->cursor - lexer->token); return T_DO; }
        "end"      { updatelloc(&(lexer->yylloc), lexer->token, lexer->cursor - lexer->token); return T_END; }
        operator   { updatelloc(&(lexer->yylloc), lexer->token, lexer->cursor - lexer->token); return T_OPERATOR; }
        identifier { updatelloc(&(lexer->yylloc), lexer->token, lexer->cursor - lexer->token); return T_IDENTIFIER; }

    */
}
