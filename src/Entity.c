#include <stdlib.h>
#include <string.h>

#include "Ast.h"
#include "Object.h"
#include "Entity.h"

void DestroyEntity(Entity* ent)
{
  DestroyObject(ent->obj);
  free(ent);
  ent = NULL;
}

Entity* CloneEntity(Entity* ent)
{
  Entity* result = (Entity*)malloc(sizeof(Entity));
  result->obj    = CloneObject(ent->obj);
  return result;
}

char* ToStringEntity(Entity* ent)
{
  char*  result = ToStringObject(ent->obj);
  return result;
}
