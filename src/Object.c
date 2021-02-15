#include <stdlib.h>
#include <string.h>

#include "PinkError.h"
#include "Ast.h"
#include "Environment.h"
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

void CloneObject(Object* dest, Object* source)
{
  dest->kind = source->kind;

  switch(source->kind)
  {
    case O_NIL:
      CloneNil(&(dest->nil), &(source->nil));
      break;
    case O_INT:
      CloneInteger(&(dest->integer), &(source->integer));
      break;
    case O_BOOL:
      CloneBoolean(&(dest->boolean), &(source->boolean));
      break;
    case O_LAMB:
      CloneLambda(&(dest->lambda), &(source->lambda));
      break;
    default:
      FatalError("Bad Object Kind", __FILE__, __LINE__);
      break;
  }
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

TypeJudgement GetypeObject(Ast* node, Environment* env)
{
  TypeJudgement result;
  switch (node->obj.kind)
  {
    case O_NIL:
    {
      result = GetypeNil(node, env);
      break;
    }

    case O_INT:
    {
      result = GetypeInteger(node, env);
      break;
    }

    case O_BOOL:
    {
      result = GetypeBoolean(node, env);
      break;
    }

    case O_LAMB:
    {
      result = GetypeLambda(node, env);
      break;
    }

    default:
    {
      FatalError("Bad Object Kind", __FILE__, __LINE__);
      break;
    }
  }
  return result;
}
