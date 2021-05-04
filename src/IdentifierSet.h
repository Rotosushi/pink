#ifndef IDENTIFIERSET_H
#define IDENTIFIERSET_H
#include <stdlib.h>
#include <StringInterner.hpp>

typedef struct IdentifierSet
{
  InternedString* ids;
  size_t          length;
} IdentifierSet;

void InsertIdentifier(IdentifierSet* set, InternedString id);

void DestroyIdentifierSet(IdentifierSet* set);


#endif // !IDENTIFIERSET_H
