#ifndef OBJECT_H
#define OBJECT_H

#include "Location.h"
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
// be doing that. Just Kidding!
// guess what my last refactor was lol.
// anyways i might also be changing things
// such that the member holds the location within
// the union. this then gives us enough information
// to connect the error to the location within
// the tree, while also allowing a more direct
// way to show how overloading in a c like
// language could work.
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

void CloneObject(Object* dest, Object* source);

char* ToStringObject(Object* obj);

TypeJudgement GetypeObject(struct Ast* node, struct Environment* env);

#endif // !OBJECT_H
