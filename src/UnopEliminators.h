#ifndef UNOPELIMINATORS_H
#define UNOPELIMINATORS_H
#include <stdlib.h>

#include "StringInterner.h"
struct Ast;
struct Type;

typedef struct Ast* (*PrimitiveUnopEliminator)(struct Ast* rval);

typedef struct UnopEliminator
{
  struct Type *rtype, *restype;
  PrimitiveUnopEliminator eliminator;
} UnopEliminator;

typedef struct UnopEliminatorList
{
  UnopEliminator* list;
  int size;
} UnopEliminatorList;

UnopEliminatorList* CreateUnopEliminatorList();
void                DestroyUnopEliminatorList(UnopEliminatorList* UElist);
UnopEliminator*     InsertPrimitiveUnopEliminator(UnopEliminatorList* UElist, struct Type* rtype, struct Type* restype, PrimitiveUnopEliminator eliminator);
UnopEliminator*     FindUnopEliminator(UnopEliminatorList* UElist, struct Type* rtype);
#endif // !UNOPELIMINATORS_H
