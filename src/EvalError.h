#ifndef EVALERROR_H
#define EVALERROR_H

#include "StringLocation.h"

typedef struct EvalError
{
  StringLocation errloc;
  char*          errdsc;
} EvalError;



#endif // !EVALERROR_H
