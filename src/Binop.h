#ifndef BINOP_H
#define BINOP_H

#include "StringInterner.h"

struct Ast;

typedef struct Binop
{
  InternedString op;
  struct Ast* lhs;
  struct Ast* rhs;
} Binop;

void DestroyBinop(Binop* bnp);

Binop* CloneBinop(Binop* binop);

char* ToStringBinop(Binop* binop);

#endif // !BINOP_H
