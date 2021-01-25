#ifndef TYPEJUDGEMENT_H
#define TYPEJUDGEMENT_H
#include <stdbool.h>

#include "Type.h"
#include "TypeError.h"

typedef struct TypeJudgement
{
  bool success;
  union {
      Type*     type;
      TypeError error;
  };
} TypeJudgement;

#endif // !TYPEJUDGEMENT_H
