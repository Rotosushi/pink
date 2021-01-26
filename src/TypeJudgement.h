#ifndef TYPEJUDGEMENT_H
#define TYPEJUDGEMENT_H
#include <stdbool.h>

#include "Type.h"
#include "PinkError.h"

typedef struct TypeJudgement
{
  bool success;
  union {
      Type*     type;
      PinkError error;
  };
} TypeJudgement;

#endif // !TYPEJUDGEMENT_H
