#include <stdlib.h>

#include "StringInterner.h"
#include "Ast.h"
#include "Type.h"
#include "UnopEliminators.h"


UnopEliminatorList* CreateUnopEliminatorList()
{
  UnopEliminatorList* result = (UnopEliminatorList*)malloc(sizeof(UnopEliminatorList));
  result->list = NULL;
  result->size = 0;
  return result;
}

void DestroyUnopEliminatorList(UnopEliminatorList* UElist)
{
  free(UElist->list);
  UElist->list = NULL;
  UElist->size = 0;
  free(UElist);
  UElist = NULL;
}

UnopEliminator* InsertPrimitiveUnopEliminator(UnopEliminatorList* UElist, Type* rtype, Type* restype, PrimitiveUnopEliminator eliminator)
{
  PrimitiveUnopEliminator prim_elim = NULL;

  if (!UElist || !rtype || !restype)
    return NULL; // fatal error: bad pointers

  UnopEliminator* result = FindUnopEliminator(UElist, rtype);

  if (!result)
  {
    UElist->size      += 1;
    UElist->list       = (UnopEliminator*)realloc(UElist->list, sizeof(UnopEliminator) * UElist->size);
    result             = &(UElist->list[UElist->size - 1]);
    result->rtype      = rtype;
    result->restype    = restype;
    result->eliminator = eliminator;
  }

  return result;
}

UnopEliminator* FindUnopEliminator(UnopEliminatorList* UElist, Type* rtype)
{
  if (!UElist || !rtype)
    return NULL; // fatal error: bad pointers

  UnopEliminator* result = NULL;

  for (int i = 0; i < UElist->size; i++)
  {
    result = &(UElist->list[i]);

    // taking advantage of interned types here
    if (result->rtype == rtype)
    {
      return result;
    }
  }

  return NULL;
}
