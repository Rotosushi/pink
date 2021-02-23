#ifndef OBJECT_H
#define OBJECT_H

#include "Location.h"
#include "Nil.h"
#include "Integer.h"
#include "Boolean.h"
#include "Lambda.h"
#include "TypeJudgement.h"
#include "TypeLiteral.h"

// so like, is this the place to store
// per value flags? like mutable,
// poison, etc?

typedef enum ObjKind
{
  O_NIL,
  O_INT,
  O_BOOL,
  O_LAMB,
  O_TYPE,
} ObjKind;


/*
  so, essentially Types are a
  description of the value(s) held
  currently within Object.
  so, why do we additionally have a
  'TypeLiteral' Object? (the core of this
  i think is, why don't we just produce the
  type based on the object in all cases? )
  well, in short to be able to bind names
  to their type during typeing.
  but the goal is to allow the types to
  be composed via 'operators'.
  each type is able to be used
  as a building block within another type.
  however the set of known operators is going
  to be constant and is known prior to
  any compilation of any program.
  so we may be able to treat type expressions in a
  different way from regular value level
  operators. (maybe they are handled as a grammar unto
  themselves?)
*/
typedef struct Object
{
  ObjKind kind;
  union {
    TypeLiteral type;
    Nil         nil;
    Integer     integer;
    Boolean     boolean;
    Lambda      lambda;
  };
} Object;

Location* GetObjLocation(Object* obj);

void DestroyObject(Object* obj);

void CloneObject(Object* dest, Object* source);

char* ToStringObject(Object* obj);

TypeJudgement GetypeObject(Object* obj, struct Environment* env);

#endif // !OBJECT_H
