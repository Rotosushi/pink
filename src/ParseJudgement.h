#ifndef PARSEJUDGEMENT_H
#define PARSEJUDGEMENT_H
#include <stdbool.h>

#include "Ast.h"
#include "ParseError.h"

typedef struct ParseJudgement
{
  bool success;
  union {
    Ast*       term;
    ParseError error;
  };
} ParseJudgement;

#endif // !PARSEJUDGEMENT_H
