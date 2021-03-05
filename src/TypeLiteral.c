
#include "Ast.h"
#include "Location.h"
#include "Environment.h"
#include "Type.h"
#include "Judgement.h"
#include "TypeLiteral.h"


void InitializeTypeLiteral(TypeLiteral* tl, Type* literal, Location* loc)
{
  tl->loc     = *loc;
  tl->literal = literal;
}

void DestroyTypeLiteral(TypeLiteral* tl)
{
  return;
}

void CloneTypeLiteral(TypeLiteral* dest, TypeLiteral* source)
{
  dest->loc     = source->loc;
  dest->literal = source->literal;
}

char* ToStringTypeLiteral(TypeLiteral* tl)
{
  return ToStringType(tl->literal);
}

Judgement GetypeTypeLiteral(TypeLiteral* tl, struct Environment* env)
{
  Judgement result;
  result.success = true;
  // note: this feels inefficient, and like it is
  //       a part of the problem of which memory is
  //       ready to be deallocated that is a major
  //       part of the problem of this kind of evaluation
  //       strategy. but we need to convert a Type* into
  //       and Ast* and the way we usually do that is wrapped
  //       within an Ast term. but here it feels like a redundant
  //       allocation.
  result.term    = CreateAstType(tl->literal, &(tl->loc));
  return result;
}

Judgement AssignTypeLiteral(TypeLiteral* dest, TypeLiteral* source)
{
  Judgement result;
  result.success = true;
  result.term    = NULL;
  dest->literal  = source->literal;
  return result;
}

Judgement EqualsTypeLiteral(TypeLiteral* t1, TypeLiteral* t2)
{
  Judgement result;
  result.success = true;
  result.term    = CreateAstBoolean(t1->literal == t2->literal, &t1->loc);
  return result;
}
