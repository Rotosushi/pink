#ifndef BINOP_H
#define BINOP_H

#include "StringInterner.h"

struct Environment;
struct Ast;

typedef struct Binop
{
  InternedString op;
  struct Ast* lhs;
  struct Ast* rhs;
} Binop;

// the repetition within the names is the first thing
// my intuition picked up on, and where i first
// saw what was my idea of fitting the type dispatch
// into a different place than OOP puts it.
// it lives in the implementation of an overloaded procedure,
// and the implementation of a polymorphic procedure.
// a pointer to either, is a pointer to the dispatch procedure.
void InitializeBinop(Binop* binop, InternedString op, struct Ast* left, struct Ast* right);

void DestroyBinop(Binop* bnp);

void CloneBinop(Binop* destination, Binop* source);

char* ToStringBinop(Binop* binop);

TypeJudgement GetypeBinop(Binop* binop, struct Environment* env);

#endif // !BINOP_H
