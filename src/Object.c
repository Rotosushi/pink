#include <stdlib.h>
#include <string.h>

#include "PinkError.h"
#include "Nil.h"
#include "Integer.h"
#include "Boolean.h"
#include "Lambda.h"
#include "Object.h"

void DestroyObject(Object* obj)
{
  switch(obj->kind)
  {
    case O_NIL:
      DestroyNil(&(obj->nil));
      break;
    case O_INT:
      DestroyInteger(&(obj->integer));
      break;
    case O_BOOL:
      DestroyBoolean(&(obj->boolean));
      break;
    case O_LAMB:
      DestroyLambda(&(obj->lambda));
      break;
    default:
      FatalError("Bad Object Kind", __FILE__, __LINE__);
      break;
  }
}

Object* CloneObject(Object* obj)
{
  Object* result = (Object*)malloc(sizeof(Object));
  result->kind = obj->kind;

  switch(result->kind)
  {
    case O_NIL:
      CloneNil(&(result->nil), &(obj->nil));
      break;
    case O_INT:
      CloneInteger(&(result->integer), &(obj->integer));
      break;
    case O_BOOL:
      CloneBoolean(&(result->integer), &(obj->boolean));
      break;
    case O_LAMB:
      CloneLambda(&(result->lambda), &(obj->lambda));
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
      result = ToStringNil(&(obj->nil));
      break;
    case O_INT:
      result = ToStringInteger(&(obj->integer));
      break;
    case O_BOOL:
      result = ToStringBoolean(&(obj->boolean));
      break;
    case O_LAMB:
      result = ToStringLambda(&(obj->lambda));
      break;
    default:
      FatalError("Bad Object Kind", __FILE__, __LINE__);
      break;
  }
  return result;
}
