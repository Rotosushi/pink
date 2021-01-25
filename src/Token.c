
#include "Token.h"


char* ToStringToken(Token tok)
{
  switch(tok)
  {
    case T_ERR:
      return "T_ERR";
    case T_END:
      return "T_END";
    case T_OPERATOR:
      return "T_OPERATOR";
    case T_IDENTIFIER:
      return "T_IDENTIFIER";
    case T_NIL:
      return "T_NIL";
    case T_INTEGER:
      return "T_INTEGER";
    case T_TRUE:
      return "T_TRUE";
    case T_FALSE:
      return "T_FALSE";
    case T_TYPE_NIL:
      return "T_TYPE_NIL";
    case T_TYPE_BOOL:
      return "T_TYPE_BOOL";
    case T_TYPE_INT:
      return "T_TYPE_INT";
    case T_BSLASH:
      return "T_BSLASH";
    case T_COLON:
      return "T_COLON";
    case T_EQRARROW:
      return "T_EQRARROW";
    case T_RARROW:
      return "T_RARROW";
    case T_COLONEQ:
      return "T_COLONEQ";
    case T_LPAREN:
      return "T_LPAREN";
    case T_RPAREN:
      return "T_RPAREN";
    case T_SEMICOLON:
      return "T_SEMICOLON";
    case T_IF:
      return "T_IF";
    case T_THEN:
      return "T_THEN";
    case T_ELSE:
      return "T_ELSE";
    case T_WHILE:
      return "T_WHILE";
    case T_DO:
      return "T_DO";
  }
  return "UNKNOWN_TOKEN";
}
