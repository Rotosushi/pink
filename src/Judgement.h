#ifndef JUDGEMENT_H
#define JUDGEMENT_H
#include <stdbool.h>

struct Ast;
#include "Error.h"

// type Judgement = Success term | Failure error
typedef struct Judgement
{
  bool success;
  union {
    struct Ast* term;
    Error   error;
  };
} Judgement;


#endif // !JUDGEMENT_H
