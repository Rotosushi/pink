#include <stdlib.h>

#include "IdentifierSet.h"


void InsertIdentifier(IdentifierSet* set, InternedString id)
{
  set->length += 1;
  set->ids = (InternedString*)realloc(set->ids, set->length);
  set->ids[set->length - 1] = id;
}

void DestroyIdentifierSet(IdentifierSet* set)
{
  free(set->ids);
  set->length = 0;
}
