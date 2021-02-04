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

BinopEliminator* InsertPrimitiveBinopEliminator(BinopEliminatorList* BElist, Type *ltype, Type *rtype, Type *restype, PrimitiveBinopEliminator PBElim)
{
  if (!BElist || !ltype || !rtype || !restype || !PBElim)
    return NULL; // i should probably write a fatal error routine for this case tbh

  BElist->size += 1;
  BElist->list = (BinopEliminator*)realloc(BElist->list, sizeof(BinopEliminator) * BElist->size);
  BinopEliminator* result = &(BElist->list[BElist->size - 1]);
  // or: (BElist->list[BElist->size - 1]).ltype = ltype;
  result->ltype = ltype;
  result->rtype = rtype;
  result->restype = restype;
  result->eliminator = PBElim;
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
    if (ltype == cur->ltype && rtype == cur->rtype)
      return cur;
  }

  return NULL;
}
