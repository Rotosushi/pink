#ifndef Error_H
#define Error_H
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "Location.h"

typedef struct Error
{
  Location loc;
  char*    dsc;
} Error;


void PrintError(FILE* out, Error* err, const char* errtxt);
void FatalError(const char* msg, const char* file, int line);

#endif // !Error_H
