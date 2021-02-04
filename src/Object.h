#ifndef OBJECT_H
#define OBJECT_H

#include "Nil.h"
#include "Integer.h"
#include "Boolean.h"
#include "Lambda.h"
#include "Type.h"

typedef enum ObjKind
{
  O_NIL,
  O_INT,
  O_BOOL,
  O_LAMB,
} ObjKind;

typedef struct Object
{
  ObjKind kind;
  union {
    Nil     *nil;
    Integer *integer;
    Boolean *boolean;
    Lambda  *lambda;
  };
} Object;

void DestroyObject(Object* obj);

Object* CloneObject(Object* obj);

char* ToStringObject(Object* obj);

#endif // !OBJECT_H
