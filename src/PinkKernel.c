#include <stdbool.h>


#include "Ast.h"
#include "PinkError.h"
#include "StringInterner.h"
#include "UnopEliminators.h"
#include "BinopEliminators.h"
#include "BinopTable.h"
#include "UnopTable.h"
#include "PinkKernel.h"

Ast* BinopPlusAddInteger(Ast* lval, Ast* rval)
{
  if (lval == NULL)
    FatalError("Cannot Add NULL left term", __FILE__, __LINE__);

  if (rval == NULL)
    FatalError("Cannot Add NULL right term", __FILE__, __LINE__);

  if (lval->kind != A_OBJ)
    FatalError("Cannot Add non-Object left term", __FILE__, __LINE__);

  if (rval->kind != A_OBJ)
    FatalError("Cannot Add non-Object right term", __FILE__, __LINE__);

  if (lval->obj.kind != O_INT)
    FatalError("Cannot Add non-Integer left Object", __FILE__, __LINE__);

  if (rval->obj.kind != O_INT)
    FatalError("Cannot Add non-Integer right Object", __FILE__, __LINE__);

  int left = lval->obj.integer.value, right = rval->obj.integer.value;

  Ast* ast = CreateAstInteger(left + right, GetAstLocation(lval));
  return ast;
}

Ast* BinopHyphenMinusInteger(Ast* lval, Ast* rval)
{
  if (lval == NULL)
    FatalError("Cannot Subtract NULL left term", __FILE__, __LINE__);

  if (rval == NULL)
    FatalError("Cannot Subtract NULL right term", __FILE__, __LINE__);

  if (lval->kind != A_OBJ)
    FatalError("Cannot Subtract non-Object left term", __FILE__, __LINE__);

  if (rval->kind != A_OBJ)
    FatalError("Cannot Subtract non-Object right term", __FILE__, __LINE__);

  if (lval->obj.kind != O_INT)
    FatalError("Cannot Subtract non-Integer left Object", __FILE__, __LINE__);

  if (rval->obj.kind != O_INT)
    FatalError("Cannot Subtract non-Integer right Object", __FILE__, __LINE__);

  int left = lval->obj.integer.value, right = rval->obj.integer.value;

  Ast* ast = CreateAstInteger(left - right, GetAstLocation(lval));
  return ast;
}

Ast* BinopStarMultiplyInteger(Ast* lval, Ast* rval)
{
  if (lval == NULL)
    FatalError("Cannot Multiply NULL left term", __FILE__, __LINE__);

  if (rval == NULL)
    FatalError("Cannot Multiply NULL right term", __FILE__, __LINE__);

  if (lval->kind != A_OBJ)
    FatalError("Cannot Multiply non-Object left term", __FILE__, __LINE__);

  if (rval->kind != A_OBJ)
    FatalError("Cannot Multiply non-Object right term", __FILE__, __LINE__);

  if (lval->obj.kind != O_INT)
    FatalError("Cannot Multiply non-Integer left Object", __FILE__, __LINE__);

  if (rval->obj.kind != O_INT)
    FatalError("Cannot Multiply non-Integer right Object", __FILE__, __LINE__);

  int left = lval->obj.integer.value, right = rval->obj.integer.value;

  Ast* ast = CreateAstInteger(left * right, GetAstLocation(lval));
  return ast;
}

Ast* BinopFslashDivideInteger(Ast* lval, Ast* rval)
{
  if (lval == NULL)
    FatalError("Cannot Divide NULL left term", __FILE__, __LINE__);

  if (rval == NULL)
    FatalError("Cannot Divide NULL right term", __FILE__, __LINE__);

  if (lval->kind != A_OBJ)
    FatalError("Cannot Divide non-Object left term", __FILE__, __LINE__);

  if (rval->kind != A_OBJ)
    FatalError("Cannot Divide non-Object right term", __FILE__, __LINE__);

  if (lval->obj.kind != O_INT)
    FatalError("Cannot Divide non-Integer left Object", __FILE__, __LINE__);

  if (rval->obj.kind != O_INT)
    FatalError("Cannot Divide non-Integer right Object", __FILE__, __LINE__);

  int left = lval->obj.integer.value, right = rval->obj.integer.value;

  Ast* ast = CreateAstInteger(left / right, GetAstLocation(lval));
  return ast;
}

Ast* UnopHyphenNegateInteger(Ast* rval)
{
  if (rval == NULL)
    FatalError("Cannot Negate NULL term", __FILE__, __LINE__);

  if (rval->kind != A_OBJ)
    FatalError("Cannot Negate non-Object term", __FILE__, __LINE__);

  if (rval->obj.kind != O_INT)
    FatalError("Cannot Negate non-Integer Object", __FILE__, __LINE__);

  Ast* ast = CreateAstInteger(-(rval->obj.integer.value), GetAstLocation(rval));
  return ast;
}

Ast* UnopBangNegateBoolean(Ast* rval)
{
  if (rval == NULL)
    FatalError("Cannot Negate NULL term", __FILE__, __LINE__);

  if (rval->kind != A_OBJ)
    FatalError("Cannot Negate non-Object term", __FILE__, __LINE__);

  if (rval->obj.kind != O_BOOL)
    FatalError("Cannot Negate non-Boolean Object", __FILE__, __LINE__);


  Ast* ast = CreateAstBoolean(!(rval->obj.boolean.value), GetAstLocation(rval));
  return ast;
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

  InternedString plusOp = InternString(env->interned_ops, "+");
  BinopEliminatorList* plusBinopElims = CreateAstBinopEliminatorList();
  InsertPrimitiveBinopEliminator(plusBinopElims, integerType, integerType, integerType, BinopPlusAddInteger);
  InsertBinopPrecAssoc(env->precedence_table, plusOp, 5, A_LEFT);
  InsertBinop(env->binops, plusOp, plusBinopElims);

  InternedString hyphenOp = InternString(env->interned_ops, "-");
  BinopEliminatorList* hyphenBinopElims = CreateAstBinopEliminatorList();
  InsertPrimitiveBinopEliminator(hyphenBinopElims, integerType, integerType, integerType, BinopHyphenMinusInteger);
  InsertBinopPrecAssoc(env->precedence_table, hyphenOp, 5, A_LEFT);
  InsertBinop(env->binops, hyphenOp, hyphenBinopElims);

  InternedString starOp = InternString(env->interned_ops, "*");
  BinopEliminatorList* starBinopElims = CreateAstBinopEliminatorList();
  InsertPrimitiveBinopEliminator(starBinopElims, integerType, integerType, integerType, BinopStarMultiplyInteger);
  InsertBinopPrecAssoc(env->precedence_table, starOp, 6, A_LEFT);
  InsertBinop(env->binops, starOp, starBinopElims);

  InternedString fslashOp = InternString(env->interned_ops, "/");
  BinopEliminatorList* fslashBinopElims = CreateAstBinopEliminatorList();
  InsertPrimitiveBinopEliminator(fslashBinopElims, integerType, integerType, integerType, BinopFslashDivideInteger);
  InsertBinopPrecAssoc(env->precedence_table, fslashOp, 6, A_LEFT);
  InsertBinop(env->binops, fslashOp, fslashBinopElims);
}
