#ifndef BINOPELIMINATORS_H
#define BINOPELIMINATORS_H
#include <stdlib.h>

#include "StringInterner.h"

struct Ast;
struct Type;

typedef struct Ast* (*PrimitiveBinopEliminator)(struct Ast* lval, struct Ast* rval);


typedef struct BinopEliminator
{
  struct Type *ltype, *rtype, *restype;
  // probably held in a union,
  // just like a judgement,
  // except Composite v. Primitive
  PrimitiveBinopEliminator eliminator;
  // Lambda* composite;
} BinopEliminator;


// array of data
typedef struct BinopEliminatorList
{
  BinopEliminator* list;
  int     size;
} BinopEliminatorList;

BinopEliminatorList* CreateBinopEliminatorList();
void                 DestroyBinopEliminatorList(BinopEliminatorList* BElist);
BinopEliminator*     InsertPrimitiveBinopEliminator(BinopEliminatorList* BElist, struct Type *ltype, struct Type *rtype, struct Type *restype, PrimitiveBinopEliminator PrimElim);
//
BinopEliminator*     FindBinopEliminator(BinopEliminatorList* BElist, struct Type *ltype, struct Type* rtype);
#endif // !BINOPELIMINATORS_H
