#ifndef PARSER_H
#define PARSER_H
#include <stdio.h>

#include "Ast.h"
#include "Location.h"
#include "Token.h"
#include "SymbolTable.h"
#include "StringInterner.h"
#include "BinopTable.h"
#include "BinopPrecedence.h"
#include "UnopTable.h"
#include "ParseJudgement.h"
#include "Lexer.h"

typedef struct Parser
{
  int*            markstack; // we can rewind through the array of tokens as many times as we want,
                             //  this stack keeps track of each 'last-valid-position' as we venture
                             //  deeper into unknown tokens, gathering them into terms.
  Token*          tokbuf;    // the buffer (array) of tokens.
  char**          txtbuf;    // the buffer of character strings which make up the tokens.
  Location*       locbuf;    // the buffer of token locations (relative to the input text.)
                             // which is always one line, because we are only interpretive.
  int             idx;       // our current index into each of our buffers.
  int             bufsz;     // the actual length of our buffers.
  int             mkstksz;   // the actual length of our mark stack.
  Lexer*          lexer;     // the lexer structure, which splits the input text into tokens.
  // these pointers memory are managed elsewhere in the program.
  SymbolTable*    outer_scope;
  StringInterner* interned_ids;
  StringInterner* interned_ops;
  BinopPrecedenceTable* precedence_table;
  BinopTable*       binops;
  UnopTable*        unops;
} Parser;

/*
  by contract, the parser manages the Lexers
  memory, but the Environments memory is
  managed above. this is because the lexer
  only ever needs to be known by the parser,
  otherwise it is wholly independant from
  the rest of the program.
*/
struct Parser* CreateParser(SymbolTable* global_scope, StringInterner* Iids, StringInterner* Iops, BinopPrecedenceTable* prec_table, BinopTable* bs, UnopTable* us);
void    DestroyParser(struct Parser* p);
void    ResetParser(struct Parser* p);
ParseJudgement Parse(struct Parser* p, char* input);
void    DumpTokens(struct Parser* p, FILE* out);


#endif //!PARSER_H
