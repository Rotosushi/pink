
#include "Location.h"
#include "Environment.h"
#include "Type.h"
#include "TypeJudgement.h"
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

TypeJudgement GetypeTypeLiteral(TypeLiteral* tl, struct Environment* env)
{
  TypeJudgement result;
  result.success = true;
  result.type    = tl->literal;
  return result;
}
