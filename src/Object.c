#include <stdlib.h>
#include <string.h>

#include "PinkError.h"
#include "Ast.h"
#include "Environment.h"
#include "Nil.h"
#include "Integer.h"
#include "Boolean.h"
#include "Lambda.h"
#include "TypeLiteral.h"
#include "Object.h"

Location* GetObjLocation(Object* obj)
{
  switch(obj->kind)
  {
    case O_TYPE:
      return &(obj->type.loc);
    case O_NIL:
      return &(obj->nil.loc);
    case O_INT:
      return &(obj->integer.loc);
    case O_BOOL:
      return &(obj->boolean.loc);
    case O_LAMB:
      return &(obj->lambda.loc);
    default:
      FatalError("Bad Object Kind", __FILE__, __LINE__);
  }
}

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
    case O_TYPE:
      DestroyTypeLiteral(&(obj->type));
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
    case O_TYPE:
      CloneTypeLiteral(&(dest->type), &(source->type));
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
    case O_TYPE:
      result = ToStringTypeLiteral(&(obj->type));
      break;
    default:
      FatalError("Bad Object Kind", __FILE__, __LINE__);
      break;
  }
  return result;
}


TypeJudgement GetypeObject(Object* obj, Environment* env)
{
  // again: "hey why aren't you just returning
  //         the type directly from this context
  //         for Nil, Int and Bool? we have all
  //         the required information to make a
  //         typeing judgement?"
  // answer: "To remind myself that this could be
  //          defined as an overloaded procedure
  //          'getype' for each member type."
  //          but how does that look exactly?
  //          and how does the union type
  //          factor into this?
  TypeJudgement result;
  switch (obj->kind)
  {
    case O_NIL:
      result = GetypeNil(&(obj->nil), env);
      break;
    case O_INT:
      result = GetypeInteger(&(obj->integer), env);
      break;
    case O_BOOL:
      result = GetypeBoolean(&(obj->boolean), env);
      break;
    case O_LAMB:
      result = GetypeLambda(&(obj->lambda), env);
      break;
    case O_TYPE:
      result = GetypeTypeLiteral(&(obj->type), env);
      break;
    default:
      FatalError("Bad Object Kind", __FILE__, __LINE__);
      break;
  }
  return result;
}
