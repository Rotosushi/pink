#ifndef REPL_H
#define REPL_H

#include <stdio.h>

#include "Environment.h"

void Repl(FILE* in, FILE* out, Environment* env);

#endif // !REPL_H
