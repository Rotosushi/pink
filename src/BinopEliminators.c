#include <stdlib.h>

#include "StringInterner.h"
#include "Ast.h"
#include "Type.h"
#include "BinopEliminators.h"

BinopEliminatorList* CreateBinopEliminatorList()
{
  BinopEliminatorList* result = (BinopEliminatorList*)malloc(sizeof(BinopEliminatorList));
  result->list = NULL;
  result->size = 0;
  return result;
}

void DestroyBinopEliminatorList(BinopEliminatorList* BElist)
{
  free(BElist->list);
  BElist->size = 0;
  free(BElist);
}

BinopEliminator* InsertPrimitiveBinopEliminator(BinopEliminatorList* BElist, Type *ltype, Type *rtype, Type *restype, PrimitiveBinopEliminator* PBElim)
{
  if (!BElist || !ltype || !rtype || !restype || !PBElim)
    return NULL; // i should probably write a fatal error routine for this case tbh

  BElist->size += 1;
  BElist->list = (BinopEliminator*)realloc(BElist->list, sizeof(BinopEliminator) * BElist->size);
  BElist->list[BElist->size - 1] = {ltype, rtype, restype, PBElim};
  return &(BElist->list[BElist->size - 1]);
}
//
BinopEliminator* FindBinopEliminator(BinopEliminatorList* BElist, Type *ltype, Type* rtype)
{
  if (!BElist || !ltype || !rtype)
    return NULL; // fatal error: unhandleable NULL ptr passed

  BinopEliminator* cur = NULL;
  for (int i = 0; i < BElist->size; i++)
  {
    cur = &(BElist->list[i]);
    if (EqualTypes(ltype, cur->ltype) && EqualTypes(rtype, cur->rtype))
      return cur;
  }

  return NULL;
}
