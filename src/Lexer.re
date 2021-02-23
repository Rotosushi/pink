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
    // pointer arithmetic is not black magic!
    // 1 number per char, regardless of the
    // upper bound, the bytes between cursor
    // (a.k.a. the end of the token.) and
    // token (a.k.a. the beginning of the token)
    // is the number of characters between
    // the two, a.k.a the length of the token.
    // [-] is one byte [----] is a word
    // "---- ---- ---- ----" is some string of
    // length 15, but this works with any length (n).
    // (held within 16 bytes.)
    //      "---- ---- ---- ----"
    //          ^        ^
    //          t        c
    // if we count the number of chars between (t, c)
    // we get 8. now, given our knowledge that a pointer
    // is the address of some character. (because it's a
    // character pointer) we know that our pointers contain
    // some integer value which can be used as an index into
    // memory to retrieve some value of type char. (which has
    // size one remember, and 4 chars make a word, and machines
    // are word addressed.) much like the idea of our pointer
    // as an index into the character array, the physical
    // representation of this index into a character array is
    // the exact same physical representation as an index
    // into our memory space.
    // so, what do you get when you
    // subtract the two indexes from eachother (t, c)?
    // you get the distance between the two, in terms of
    // the underlying type. (char is easy because it is
    // quite literally defined to be both length 1, w.r.t
    // the size of everything in a C program, and to be
    // the length of one byte of memory, on essentially
    // all modern machines, because of the nice tricks it allows.)
    // so truly the question we need to ask to ensure we don't
    // get a negative length is which pointer is the smaller number?
    // luckily we read the documentation of re2c
    // and so we know that cursor is the seeking pointer, i.e. it
    // is bumped along the string as we lex each character,
    // and we know that token is the beginning of our current token.
    // hence, cursor is always greater than or equal to token.
    // all you need to know more than this about pointer arithmetic
    // is that all of this is in terms of n elements of size m.
    // where char is our anchor/bridge between the machine and
    // our type system.
    // we think arrays and pointers into arrays as
    // offsets, however we multiply by the size of each element
    // first, to convert from byte indexing to element indexing.
    //
    // notice how this is essentially the same thing as scale conversion!
    // we talk about the size of every type in terms of how many bytes
    // it takes to store an object/entity of that type. (where char is
    // the bridge between bytes and the type system.)
    //
    // if that is confusing, thing of some magical struct
    // of size 10, (in bytes remember), if we want an array of
    // ten of these how much memory do we need?
    // 10 (size of struct) * 10 (number of elements) = 100 (bytes)
    // and, where is the sixth struct within the array?
    // 10 (size of struct) * 6  (which element we want/index) = 60 (th byte)
    // hence, the array derefrence operator is exactly the same as
    // adding some number to a pointer to construct a new pointer.
    // and, the dot operator is also pointer arithmetic in disguise.
    // if a struct is ten bytes, composed of five members each of
    // size two bytes, then to access the third member means accessing
    // the two bytes immediately following the first four bytes of the structure.
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
