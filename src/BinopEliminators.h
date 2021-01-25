#include <stdlib.h>

#include "StringInterner.h"
#include "Ast.h"
#include "Type.h"

typedef Ast* (*PrimitiveBinopEliminator)(Ast* lval, Ast* rval);


typedef struct BinopEliminator
{
  Type *ltype, *rtype, *restype;
  // probably held in a union,
  // just like a judgement,
  // except Composite v. Primitive
  PrimitiveBinopEliminator primitive;
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
BinopEliminator*     InsertPrimitiveBinopEliminator(BinopEliminatorList* BElist, Type *ltype, Type *rtype, Type *restype, PrimitiveBinopEliminator* PrimElim);
//
BinopEliminator*     FindBinopEliminator(BinopEliminatorList* BElist, Type *ltype, Type* rtype);
