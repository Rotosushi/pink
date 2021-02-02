#ifndef TOKEN_H
#define TOKEN_H

typedef enum Token
{
  T_ERR,
  T_EOF,
  T_END,
  T_OPERATOR,
  T_IDENTIFIER,
  T_NIL,
  T_INTEGER,
  T_TRUE,
  T_FALSE,
  T_TYPE_NIL,
  T_TYPE_BOOL,
  T_TYPE_INT,
  T_BSLASH,
  T_COLON,
  T_EQRARROW,
  T_RARROW,
  T_LARROW,
  T_COLONEQ,
  T_LPAREN,
  T_RPAREN,
  T_SEMICOLON,
  T_IF,
  T_THEN,
  T_ELSE,
  T_WHILE,
  T_DO,
} Token;

char* ToStringToken(Token tok);

#endif // !TOKEN_H
