#ifndef PARSEJUDGEMENT_H
#define PARSEJUDGEMENT_H
#include <stdbool.h>

#include "Ast.h"
#include "PinkError.h"

typedef struct ParseJudgement
{
  bool success;
  union {
    struct Ast* term;
    PinkError   error;
  };
} ParseJudgement;

#endif // !PARSEJUDGEMENT_H
