#ifndef PINKERROR_H
#define PINKERROR_H
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "Location.h"

typedef struct PinkError
{
  Location loc;
  char*    dsc;
} PinkError;


void PrintError(FILE* out, PinkError* err, const char* errtxt);
void FatalError(const char* msg, const char* file, int line);

#endif // !PINKERROR_H
