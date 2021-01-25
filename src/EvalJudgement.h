#ifndef EVALJUDGEMENT_H
#define EVALJUDGEMENT_H
#include <stdbool.h>

#include "Ast.h"
#include "EvalError.h"

typedef struct EvalJudgement
{
  bool success;
  union {
    Ast*      term;
    EvalError error;
  };
} EvalJudgement;


#endif // !EVALJUDGEMENT_H
