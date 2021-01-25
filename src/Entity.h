#ifndef ENTITY_H
#define ENTITY_H

#include "Object.h"

typedef struct Entity
{
  Object* obj;
} Entity;

void DestroyEntity(Entity* ent);

Entity* CloneEntity(Entity* ent);

char* ToStringEntity(Entity* ent);

#endif // !ENTITY_H
