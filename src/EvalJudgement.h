#ifndef EVALJUDGEMENT_H
#define EVALJUDGEMENT_H
#include <stdbool.h>

struct Ast;
#include "PinkError.h"

typedef struct EvalJudgement
{
  bool success;
  union {
    struct Ast*      term;
    PinkError error;
  };
} EvalJudgement;


#endif // !EVALJUDGEMENT_H
