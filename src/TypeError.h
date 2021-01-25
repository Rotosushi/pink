#ifndef TYPEERROR_H
#define TYPEERROR_H

#include "StringLocation.h"

typedef struct TypeError
{
  StringLocation errloc;
  char*          errdsc;
} TypeError;

#endif // !TYPEERROR_H
