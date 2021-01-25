#ifndef PARSEERROR_H
#define PARSEERROR_H

#include "StringLocation.h"

typedef struct ParseError
{
  StringLocation errloc;
  char*          errdsc;
} ParseError;

#endif // !PARSEERROR_H
