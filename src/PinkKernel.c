#include <stdbool.h>


#include "Ast.h"
#include "Utilities.hpp"
#include "Error.h"
#include "StringInterner.hpp"
#include "UnopEliminators.h"
#include "BinopEliminators.h"
#include "BinopTable.h"
#include "UnopTable.h"
#include "PinkKernel.h"

Judgement BinopEquals(Ast* lval, Ast* rval)
{
  Judgement eqjdgmt = Equals(lval, rval);
  return eqjdgmt;
}

Judgement BinopPlusAddInteger(Ast* lval, Ast* rval)
{
  Judgement result;
  if (lval == NULL)
    FatalError("Cannot Add NULL left term", __FILE__, __LINE__);

  if (rval == NULL)
    FatalError("Cannot Add NULL right term", __FILE__, __LINE__);

  if (lval->kind != A_OBJ)
  {
    result.success   = false;
    result.error.loc = *GetAstLocation(lval);
    result.error.dsc = dupstr("Cannot Add non-Object left term");
    return result;
  }

  if (rval->kind != A_OBJ)
  {
    result.success   = false;
    result.error.loc = *GetAstLocation(rval);
    result.error.dsc = dupstr("Cannot Add non-Object right term");
    return result;
  }

  if (lval->obj.kind != O_INT)
  {
    result.success   = false;
    result.error.loc = *GetAstLocation(lval);
    result.error.dsc = dupstr("Cannot Add non-Integer left term");
    return result;
  }

  if (rval->obj.kind != O_INT)
  {
    result.success   = false;
    result.error.loc = *GetAstLocation(rval);
    result.error.dsc = dupstr("Cannot Add non-Integer right term");
    return result;
  }
  int left = lval->obj.integer.value, right = rval->obj.integer.value;

  result.success = true;
  result.term    = CreateAstInteger(left + right, GetAstLocation(lval));
  return result;
}

Judgement BinopHyphenMinusInteger(Ast* lval, Ast* rval)
{
  Judgement result;
  if (lval == NULL)
    FatalError("Cannot Subtract NULL left term", __FILE__, __LINE__);

  if (rval == NULL)
    FatalError("Cannot Subtract NULL right term", __FILE__, __LINE__);

  if (lval->kind != A_OBJ)
  {
    result.success   = false;
    result.error.loc = *GetAstLocation(lval);
    result.error.dsc = dupstr("Cannot Subtract non-Object left term");
    return result;
  }

  if (rval->kind != A_OBJ)
  {
    result.success   = false;
    result.error.loc = *GetAstLocation(rval);
    result.error.dsc = dupstr("Cannot Subtract non-Object right term");
    return result;
  }

  if (lval->obj.kind != O_INT)
  {
    result.success   = false;
    result.error.loc = *GetAstLocation(lval);
    result.error.dsc = dupstr("Cannot Subtract non-Integer left term");
    return result;
  }

  if (rval->obj.kind != O_INT)
  {
    result.success   = false;
    result.error.loc = *GetAstLocation(rval);
    result.error.dsc = dupstr("Cannot Subtract non-Integer right term");
    return result;
  }
  int left = lval->obj.integer.value, right = rval->obj.integer.value;

  result.success = true;
  result.term    = CreateAstInteger(left - right, GetAstLocation(lval));
  return result;
}

Judgement BinopStarMultiplyInteger(Ast* lval, Ast* rval)
{
  Judgement result;
  if (lval == NULL)
    FatalError("Cannot Multiply NULL left term", __FILE__, __LINE__);

  if (rval == NULL)
    FatalError("Cannot Multiply NULL right term", __FILE__, __LINE__);

  if (lval->kind != A_OBJ)
  {
    result.success   = false;
    result.error.loc = *GetAstLocation(lval);
    result.error.dsc = dupstr("Cannot Multiply non-Object left term");
    return result;
  }

  if (rval->kind != A_OBJ)
  {
    result.success   = false;
    result.error.loc = *GetAstLocation(rval);
    result.error.dsc = dupstr("Cannot Multiply non-Object right term");
    return result;
  }

  if (lval->obj.kind != O_INT)
  {
    result.success   = false;
    result.error.loc = *GetAstLocation(lval);
    result.error.dsc = dupstr("Cannot Multiply non-Integer left term");
    return result;
  }

  if (rval->obj.kind != O_INT)
  {
    result.success   = false;
    result.error.loc = *GetAstLocation(rval);
    result.error.dsc = dupstr("Cannot Multiply non-Integer right term");
    return result;
  }
  int left = lval->obj.integer.value, right = rval->obj.integer.value;

  result.success = true;
  result.term    = CreateAstInteger(left * right, GetAstLocation(lval));
  return result;
}

Judgement BinopFslashDivideInteger(Ast* lval, Ast* rval)
{
  Judgement result;
  if (lval == NULL)
    FatalError("Cannot Divide NULL left term", __FILE__, __LINE__);

  if (rval == NULL)
    FatalError("Cannot Divide NULL right term", __FILE__, __LINE__);

  if (lval->kind != A_OBJ)
  {
    result.success   = false;
    result.error.loc = *GetAstLocation(lval);
    result.error.dsc = dupstr("Cannot Divide non-Object left term");
    return result;
  }

  if (rval->kind != A_OBJ)
  {
    result.success   = false;
    result.error.loc = *GetAstLocation(rval);
    result.error.dsc = dupstr("Cannot Divide non-Object right term");
    return result;
  }

  if (lval->obj.kind != O_INT)
  {
    result.success   = false;
    result.error.loc = *GetAstLocation(lval);
    result.error.dsc = dupstr("Cannot Divide non-Integer left term");
    return result;
  }

  if (rval->obj.kind != O_INT)
  {
    result.success   = false;
    result.error.loc = *GetAstLocation(rval);
    result.error.dsc = dupstr("Cannot Divide non-Integer right term");
    return result;
  }
  int left = lval->obj.integer.value, right = rval->obj.integer.value;

  result.success = true;
  result.term    = CreateAstInteger(left / right, GetAstLocation(lval));
  return result;
}

Judgement UnopHyphenNegateInteger(Ast* rval)
{
  Judgement result;
  if (rval == NULL)
    FatalError("Cannot Negate NULL term", __FILE__, __LINE__);

  if (rval->kind != A_OBJ)
  {
    result.success   = false;
    result.error.loc = *GetAstLocation(rval);
    result.error.dsc = dupstr("Cannot Negate non-Object term");
    return result;
  }

  if (rval->obj.kind != O_INT)
  {
    result.success = false;
    result.error.loc = *GetAstLocation(rval);
    result.error.dsc = dupstr("Cannot Negate non-Integer Object");
    return result;
  }

  result.success = true;
  result.term    = CreateAstInteger(-(rval->obj.integer.value), GetAstLocation(rval));
  return result;
}

Judgement UnopBangNegateBoolean(Ast* rval)
{
  Judgement result;
  if (rval == NULL)
    FatalError("Cannot Negate NULL term", __FILE__, __LINE__);

  if (rval->kind != A_OBJ)
  {
    result.success   = false;
    result.error.loc = *GetAstLocation(rval);
    result.error.dsc = dupstr("Cannot Negate non-Object term");
    return result;
  }

  if (rval->obj.kind != O_BOOL)
  {
    result.success   = false;
    result.error.loc = *GetAstLocation(rval);
    result.error.dsc = dupstr("Cannot Negate non-Boolean Object");
    return result;
  }

  result.success = true;
  result.term    = CreateAstBoolean(!(rval->obj.boolean.value), GetAstLocation(rval));
  return result;
}

void RegisterPrimitiveUnops(Environment* env)
{
  Type* booleanType = GetBooleanType(env->interned_types);
  Type* integerType = GetIntegerType(env->interned_types);
  InternedString hyphenOp = InternString(env->interned_ops, "-");
  UnopEliminatorList* hyphenUnopElims = CreateUnopEliminatorList();
  InsertPrimitiveUnopEliminator(hyphenUnopElims, integerType, integerType, UnopHyphenNegateInteger);
  InsertUnop(env->unops, hyphenOp, hyphenUnopElims);

  InternedString bangOp = InternString(env->interned_ops, "!");
  UnopEliminatorList* bangUnopElims = CreateUnopEliminatorList();
  InsertPrimitiveUnopEliminator(bangUnopElims, booleanType, booleanType, UnopBangNegateBoolean);
  InsertUnop(env->unops, bangOp, bangUnopElims);
}

void RegisterPrimitiveBinops(Environment* env)
{
  Type* booleanType = GetBooleanType(env->interned_types);
  Type* integerType = GetIntegerType(env->interned_types);
  Type* nilType     = GetNilType(env->interned_types);

  InternedString eqOp = InternString(env->interned_ops, "=");
  BinopEliminatorList* eqBinopElims = CreateBinopEliminatorList();
  InsertPrimitiveBinopEliminator(eqBinopElims, integerType, integerType, booleanType, BinopEquals);
  InsertPrimitiveBinopEliminator(eqBinopElims, booleanType, booleanType, booleanType, BinopEquals);
  InsertPrimitiveBinopEliminator(eqBinopElims, nilType,     nilType,     booleanType, BinopEquals);
  InsertBinopPrecAssoc(env->precedence_table, eqOp, 1, A_LEFT);
  InsertBinop(env->binops, eqOp, eqBinopElims);

  InternedString plusOp = InternString(env->interned_ops, "+");
  BinopEliminatorList* plusBinopElims = CreateBinopEliminatorList();
  InsertPrimitiveBinopEliminator(plusBinopElims, integerType, integerType, integerType, BinopPlusAddInteger);
  InsertBinopPrecAssoc(env->precedence_table, plusOp, 5, A_LEFT);
  InsertBinop(env->binops, plusOp, plusBinopElims);

  InternedString hyphenOp = InternString(env->interned_ops, "-");
  BinopEliminatorList* hyphenBinopElims = CreateBinopEliminatorList();
  InsertPrimitiveBinopEliminator(hyphenBinopElims, integerType, integerType, integerType, BinopHyphenMinusInteger);
  InsertBinopPrecAssoc(env->precedence_table, hyphenOp, 5, A_LEFT);
  InsertBinop(env->binops, hyphenOp, hyphenBinopElims);

  InternedString starOp = InternString(env->interned_ops, "*");
  BinopEliminatorList* starBinopElims = CreateBinopEliminatorList();
  InsertPrimitiveBinopEliminator(starBinopElims, integerType, integerType, integerType, BinopStarMultiplyInteger);
  InsertBinopPrecAssoc(env->precedence_table, starOp, 6, A_LEFT);
  InsertBinop(env->binops, starOp, starBinopElims);

  InternedString fslashOp = InternString(env->interned_ops, "/");
  BinopEliminatorList* fslashBinopElims = CreateBinopEliminatorList();
  InsertPrimitiveBinopEliminator(fslashBinopElims, integerType, integerType, integerType, BinopFslashDivideInteger);
  InsertBinopPrecAssoc(env->precedence_table, fslashOp, 6, A_LEFT);
  InsertBinop(env->binops, fslashOp, fslashBinopElims);
}
