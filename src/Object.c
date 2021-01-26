#include <stdlib.h>
#include <string.h>

#include "PinkError.h"
#include "Nil.h"
#include "Integer.h"
#include "Boolean.h"
#include "Lambda.h"
#include "Type.h"
#include "Object.h"

void DestroyObject(Object* obj)
{
  switch(obj->kind)
  {
    case O_NIL:
      DestroyNil(obj->nil);
      break;
    case O_INT:
      DestroyInteger(obj->integer);
      break;
    case O_BOOL:
      DestroyBoolean(obj->boolean);
      break;
    case O_PROC:
      DestroyLambda(obj->proc);
      break;
    case O_TYPE:
      DestroyType(obj->type);
      break;
    default:
      FatalError("Bad Object Kind", __FILE__, __LINE__);
      break;
  }
  free(obj);
  obj = NULL;
}

Object* CloneObject(Object* obj)
{
  Object* result = (Object*)malloc(sizeof(Object));
  result->kind = obj->kind;

  switch(result->kind)
  {
    case O_NIL:
      result->nil = CloneNil(obj->nil);
      break;
    case O_INT:
      result->integer = CloneInteger(obj->integer);
      break;
    case O_BOOL:
      result->boolean = CloneBoolean(obj->boolean);
      break;
    case O_PROC:
      result->proc = CloneLambda(obj->proc);
      break;
    case O_TYPE:
      result->type = CloneType(obj->type);
      break;
    default:
      FatalError("Bad Object Kind", __FILE__, __LINE__);
      break;
  }
  return result;
}

char* ToStringObject(Object* obj)
{
  char *result;
  switch (obj->kind)
  {
    case O_NIL:
      result = ToStringNil(obj->nil);
      break;
    case O_INT:
      result = ToStringInteger(obj->integer);
      break;
    case O_BOOL:
      result = ToStringBoolean(obj->boolean);
      break;
    case O_PROC:
      result = ToStringLambda(obj->proc);
      break;
    case O_TYPE:
      result = ToStringType(obj->type);
      break;
    default:
      FatalError("Bad Object Kind", __FILE__, __LINE__);
      break;
  }
  return result;
}
