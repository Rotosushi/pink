#ifndef BINOP_H
#define BINOP_H

#include "StringInterner.h"
#include "Judgement.h"
#include "Location.h"
struct Environment;
struct Ast;

typedef struct Binop
{
  Location       loc;
  InternedString op;
  struct Ast*    lhs;
  struct Ast*    rhs;
} Binop;

// the repetition within the names is the first thing
// my intuition picked up on, and where i first
// saw what was my idea of fitting the type dispatch
// into a different place than OOP puts it.
// it lives in the implementation of an overloaded procedure,
// and the implementation of a polymorphic procedure.
// a pointer to either, is a pointer to the dispatch procedure.
void InitializeBinop(Binop* binop, InternedString op, struct Ast* left, struct Ast* right, Location* loc);

void DestroyBinop(Binop* binop);

void CloneBinop(Binop* destination, Binop* source);

char* ToStringBinop(Binop* binop);

Judgement GetypeBinop(Binop* binop, struct Environment* env);

Judgement EvaluateBinop(Binop* binop, struct Environment* env);

bool AppearsFreeBinop(Binop* binop, InternedString id);

void RenameBindingBinop(Binop* binop, InternedString target, InternedString replacement);

void SubstituteBinop(Binop* binop, struct Ast** target, InternedString id, struct Ast* value, struct Environment* env);

#endif // !BINOP_H
