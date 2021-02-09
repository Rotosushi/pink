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

// it is essentially possible to
// modify this code such that
// object is the union of all of
// the types directly. (and the
// same goes for Ast as well)
// however, that has deep implications
// within the entirety of the
// program I just wrote. so i won't
// be doing that.
typedef struct Object
{
  ObjKind kind;
  union {
    Nil     nil;
    Integer integer;
    Boolean boolean;
    Lambda  lambda;
  };
} Object;

void DestroyObject(Object* obj);

Object* CloneObject(Object* obj);

char* ToStringObject(Object* obj);

#endif // !OBJECT_H
