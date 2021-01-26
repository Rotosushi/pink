#ifndef EVALERROR_H
#define EVALERROR_H

#include "Location.h"

typedef struct EvalError
{
  Location errloc;
  char*    errdsc;
} EvalError;



#endif // !EVALERROR_H
