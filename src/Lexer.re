#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "Token.h"
#include "Lexer.h"


Lexer* CreateLexer(FILE* in)
{
  Lexer* lxr = (Lexer*)malloc(sizeof(Lexer));
  memset(lxr->buf, 0, LEXER_BUF_SZ + 1);
  lxr->yych     = 0;
  lxr->end      = lxr->cursor = lxr->token = lxr->marker = lxr->buf + LEXER_BUF_SZ;
  lxr->yyaccept = 0;
  lxr->state    = -1;
  if (in)
  {
    if (in == stdin)
      lxr->is_stdin = true;
    else
      lxr->is_stdin = false;

    lxr->yyin = in;
  }
  else
  {
    lxr->yyin     = stdin;
    lxr->is_stdin = true;
  }
}

void DestroyLexer(Lexer* lexer)
{
    // this is only safe because the
    // lexer is defined as holding a
    // static sized array, meaning no
    // dynamic-allocation occurs
    // within the lexer.
    free(lexer);
}

void yySetBuffer(Lexer* lexer, char* text, int len)
{
    if (len > LEXER_BUF_SZ)
        error_abort("cannot buffer input text, aborting");

    memset(lexer->buf, 0, LEXER_BUF_SZ);
    memcpy(lexer->buf, text, len);

    lexer->cursor = lexer->marker = lexer->mrkctx = lexer->token = lexer->buf;
    lexer->end = lexer->buf + len;
}

char* yyText(Lexer* lexer)
{
    return strndup(lexer->token, lexer->cursor - lexer->token);
}

StringLocation* yyLloc(Lexer* lexer)
{
    return &(lexer->yylloc);
}

void updatelloc(StringLocation* llocp, char* token, int len)
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
    operator   = [+\-*/%<>:=&@!~|$\^]+;
*/

#define YYPEEK() *lexer->cursor
#define YYSKIP() ++(scanner->cursor)
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

        * { updatelloc(lexer->yylloc, lexer->cursor - lexer->token); return T_ERR; }
        $ { updatelloc(lexer->yylloc, lexer->cursor - lexer->token); return T_END; }
        [ \t\n]    { updatelloc(lexer->yylloc, lexer->cursor - lexer->token); goto LOOP; }
        operator   { updatelloc(lexer->yylloc, lexer->cursor - lexer->token); return T_OPERATOR; }
        identifier { updatelloc(lexer->yylloc, lexer->cursor - lexer->token); return T_IDENTIFIER; }
        "nil"      { updatelloc(lexer->yylloc, lexer->cursor - lexer->token); return T_NIL; }
        "Nil"      { updatelloc(lexer->yylloc, lexer->cursor - lexer->token); return T_TYPE_NIL; }
        "true"     { updatelloc(lexer->yylloc, lexer->cursor - lexer->token); return T_TRUE; }
        "false"    { updatelloc(lexer->yylloc, lexer->cursor - lexer->token); return T_FALSE; }
        "Bool"     { updatelloc(lexer->yylloc, lexer->cursor - lexer->token); return T_TYPE_BOOL; }
        integer    { updatelloc(lexer->yylloc, lexer->cursor - lexer->token); return T_INTEGER; }
        "Int"      { updatelloc(lexer->yylloc, lexer->cursor - lexer->token); return T_TYPE_INT; }
        "\\"       { updatelloc(lexer->yylloc, lexer->cursor - lexer->token); return T_BSLASH; }
        ":"        { updatelloc(lexer->yylloc, lexer->cursor - lexer->token); return T_COLON; }
        "=>"       { updatelloc(lexer->yylloc, lexer->cursor - lexer->token); return T_EQRARROW; }
        "->"       { updatelloc(lexer->yylloc, lexer->cursor - lexer->token); return T_RARROW; }
        "<-"       { updatelloc(lexer->yylloc, lexer->cursor - lexer->token); return T_LARROW; }
        ":="       { updatelloc(lexer->yylloc, lexer->cursor - lexer->token); return T_COLONEQ; }
        "("        { updatelloc(lexer->yylloc, lexer->cursor - lexer->token); return T_LPAREN; }
        ")"        { updatelloc(lexer->yylloc, lexer->cursor - lexer->token); return T_RPAREN; }
        ";"        { updatelloc(lexer->yylloc, lexer->cursor - lexer->token); return T_SEMICOLON; }
        "if"       { updatelloc(lexer->yylloc, lexer->cursor - lexer->token); return T_IF; }
        "then"     { updatelloc(lexer->yylloc, lexer->cursor - lexer->token); return T_THEN; }
        "else"     { updatelloc(lexer->yylloc, lexer->cursor - lexer->token); return T_ELSE; }
        "while"    { updatelloc(lexer->yylloc, lexer->cursor - lexer->token); return T_WHILE; }
        "do"       { updatelloc(lexer->yylloc, lexer->cursor - lexer->token); return T_DO; }

    */
}
