
#include "Token.h"
#include "Utilities.h"

char* ToStringToken(Token tok)
{
  switch(tok)
  {
    case T_ERR:
      return dupstr("T_ERR");
    case T_EOF:
      return dupstr("T_EOF");
    case T_END:
      return dupstr("T_END");
    case T_OPERATOR:
      return dupstr("T_OPERATOR");
    case T_IDENTIFIER:
      return dupstr("T_IDENTIFIER");
    case T_NIL:
      return dupstr("T_NIL");
    case T_INTEGER:
      return dupstr("T_INTEGER");
    case T_TRUE:
      return dupstr("T_TRUE");
    case T_FALSE:
      return dupstr("T_FALSE");
    case T_TYPE_NIL:
      return dupstr("T_TYPE_NIL");
    case T_TYPE_BOOL:
      return dupstr("T_TYPE_BOOL");
    case T_TYPE_INT:
      return dupstr("T_TYPE_INT");
    case T_BSLASH:
      return dupstr("T_BSLASH");
    case T_COLON:
      return dupstr("T_COLON");
    case T_EQRARROW:
      return dupstr("T_EQRARROW");
    case T_RARROW:
      return dupstr("T_RARROW");
    case T_LARROW:
      return dupstr("T_LARROW");
    case T_COLONEQ:
      return dupstr("T_COLONEQ");
    case T_LPAREN:
      return dupstr("T_LPAREN");
    case T_RPAREN:
      return dupstr("T_RPAREN");
    case T_SEMICOLON:
      return dupstr("T_SEMICOLON");
    case T_IF:
      return dupstr("T_IF");
    case T_THEN:
      return dupstr("T_THEN");
    case T_ELSE:
      return dupstr("T_ELSE");
    case T_WHILE:
      return dupstr("T_WHILE");
    case T_DO:
      return dupstr("T_DO");
  }
  return dupstr("T_UNKNOWN");
}
