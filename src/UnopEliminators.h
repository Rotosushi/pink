#ifndef UNOPELIMINATORS_H
#define UNOPELIMINATORS_H
#include <stdlib.h>

#include "StringInterner.h"
#include "Ast.h"
#include "Type.h"

typedef struct Ast* (*PrimitiveUnopEliminator)(struct Ast* rval);

typedef struct UnopEliminator
{
  Type *rtype, *restype;
  PrimitiveUnopEliminator eliminator;
} UnopEliminator;

typedef struct UnopEliminatorList
{
  UnopEliminator* list;
  int size;
} UnopEliminatorList;

UnopEliminatorList* CreateUnopEliminatorList();
void                DestroyUnopEliminatorList(UnopEliminatorList* UElist);
UnopEliminator*     InsertPrimitiveUnopEliminator(UnopEliminatorList* UElist, Type* rtype, Type* restype, PrimitiveUnopEliminator eliminator);
UnopEliminator*     FindUnopEliminator(UnopEliminatorList* UElist, Type* rtype);
#endif // !UNOPELIMINATORS_H
