#include "kernel/ops/BinopPrimitives.h"
#include "ops/BinopTable.h"

namespace pink {
auto BinopIntAdd(llvm::Type *lty, llvm::Value *left, llvm::Type *rty,
                 llvm::Value *right, const Environment &env)
    -> Outcome<llvm::Value *, Error> {
  assert(lty != nullptr);
  assert(rty != nullptr);
  assert(left != nullptr);
  assert(right != nullptr);

  return {env.instruction_builder->CreateAdd(left, right, "iadd")};
}

auto BinopIntSub(llvm::Type *lty, llvm::Value *left, llvm::Type *rty,
                 llvm::Value *right, const Environment &env)
    -> Outcome<llvm::Value *, Error> {
  assert(lty != nullptr);
  assert(rty != nullptr);
  assert(left != nullptr);
  assert(right != nullptr);

  return {env.instruction_builder->CreateSub(left, right, "isub")};
}

auto BinopIntMul(llvm::Type *lty, llvm::Value *left, llvm::Type *rty,
                 llvm::Value *right, const Environment &env)
    -> Outcome<llvm::Value *, Error> {
  assert(lty != nullptr);
  assert(rty != nullptr);
  assert(left != nullptr);
  assert(right != nullptr);

  return {env.instruction_builder->CreateMul(left, right, "imul")};
}

auto BinopIntSDiv(llvm::Type *lty, llvm::Value *left, llvm::Type *rty,
                  llvm::Value *right, const Environment &env)
    -> Outcome<llvm::Value *, Error> {
  assert(lty != nullptr);
  assert(rty != nullptr);
  assert(left != nullptr);
  assert(right != nullptr);

  return {env.instruction_builder->CreateSDiv(left, right, "idiv")};
}

auto BinopIntMod(llvm::Type *lty, llvm::Value *left, llvm::Type *rty,
                 llvm::Value *right, const Environment &env)
    -> Outcome<llvm::Value *, Error> {
  assert(lty != nullptr);
  assert(rty != nullptr);
  assert(left != nullptr);
  assert(right != nullptr);

  return {env.instruction_builder->CreateSRem(left, right, "imod")};
}

auto BinopIntEq(llvm::Type *lty, llvm::Value *left, llvm::Type *rty,
                llvm::Value *right, const Environment &env)
    -> Outcome<llvm::Value *, Error> {
  assert(lty != nullptr);
  assert(rty != nullptr);
  assert(left != nullptr);
  assert(right != nullptr);

  return {env.instruction_builder->CreateICmpEQ(left, right, "ieq")};
}

auto BinopBoolEq(llvm::Type *lty, llvm::Value *left, llvm::Type *rty,
                 llvm::Value *right, const Environment &env)
    -> Outcome<llvm::Value *, Error> {
  assert(lty != nullptr);
  assert(rty != nullptr);
  assert(left != nullptr);
  assert(right != nullptr);

  return {env.instruction_builder->CreateICmpEQ(left, right, "booleq")};
}

auto BinopIntNe(llvm::Type *lty, llvm::Value *left, llvm::Type *rty,
                llvm::Value *right, const Environment &env)
    -> Outcome<llvm::Value *, Error> {
  assert(lty != nullptr);
  assert(rty != nullptr);
  assert(left != nullptr);
  assert(right != nullptr);

  return {env.instruction_builder->CreateICmpNE(left, right, "ineq")};
}

auto BinopBoolNe(llvm::Type *lty, llvm::Value *left, llvm::Type *rty,
                 llvm::Value *right, const Environment &env)
    -> Outcome<llvm::Value *, Error> {
  assert(lty != nullptr);
  assert(rty != nullptr);
  assert(left != nullptr);
  assert(right != nullptr);

  return {env.instruction_builder->CreateICmpNE(left, right, "boolneq")};
}

auto BinopIntGt(llvm::Type *lty, llvm::Value *left, llvm::Type *rty,
                llvm::Value *right, const Environment &env)
    -> Outcome<llvm::Value *, Error> {
  assert(lty != nullptr);
  assert(rty != nullptr);
  assert(left != nullptr);
  assert(right != nullptr);

  return {env.instruction_builder->CreateICmpSGT(left, right, "igt")};
}

auto BinopIntGe(llvm::Type *lty, llvm::Value *left, llvm::Type *rty,
                llvm::Value *right, const Environment &env)
    -> Outcome<llvm::Value *, Error> {
  assert(lty != nullptr);
  assert(rty != nullptr);
  assert(left != nullptr);
  assert(right != nullptr);

  return {env.instruction_builder->CreateICmpSGE(left, right, "ige")};
}

auto BinopIntLt(llvm::Type *lty, llvm::Value *left, llvm::Type *rty,
                llvm::Value *right, const Environment &env)
    -> Outcome<llvm::Value *, Error> {
  assert(lty != nullptr);
  assert(rty != nullptr);
  assert(left != nullptr);
  assert(right != nullptr);

  return {env.instruction_builder->CreateICmpSLT(left, right, "ilt")};
}

auto BinopIntLe(llvm::Type *lty, llvm::Value *left, llvm::Type *rty,
                llvm::Value *right, const Environment &env)
    -> Outcome<llvm::Value *, Error> {
  assert(lty != nullptr);
  assert(rty != nullptr);
  assert(left != nullptr);
  assert(right != nullptr);

  return {env.instruction_builder->CreateICmpSLE(left, right, "ile")};
}

auto BinopBoolAnd(llvm::Type *lty, llvm::Value *left, llvm::Type *rty,
                  llvm::Value *right, const Environment &env)
    -> Outcome<llvm::Value *, Error> {
  assert(lty != nullptr);
  assert(rty != nullptr);
  assert(left != nullptr);
  assert(right != nullptr);

  return {env.instruction_builder->CreateAnd(left, right, "and")};
}

auto BinopBoolOr(llvm::Type *lty, llvm::Value *left, llvm::Type *rty,
                 llvm::Value *right, const Environment &env)
    -> Outcome<llvm::Value *, Error> {
  assert(lty != nullptr);
  assert(rty != nullptr);
  assert(left != nullptr);
  assert(right != nullptr);

  return {env.instruction_builder->CreateOr(left, right, "or")};
}

auto BinopPtrLeftAdd(llvm::Type *lty, llvm::Value *left, llvm::Type *rty,
                     llvm::Value *right, const Environment &env)
    -> Outcome<llvm::Value *, Error> {
  /*
   * compute the address of the i'th element in the array pointed to
   * by left. where right holds the value of i. We consider an array
   * to decompose into a simple pointer to it's zero'th offset.
   * this means that given a pointer to a simple type, we only need to
   * compute a GEP 1, and not a GEP 2. because the offset relative to
   * the pointer will be the pointee type offset from the original location.
   * this differs from how we would index a pointer to an array, because when
   * we compute the first offset away from a pointer to an array type, we
   * would be computing the offset of the first array after the end of the
   * given array.
   */
  assert(lty != nullptr);
  assert(rty != nullptr);
  assert(left != nullptr);
  assert(right != nullptr);

  std::vector<llvm::Value *> indicies = {right};
  return {env.instruction_builder->CreateGEP(lty, left, indicies,
                                             "gep" /* isInBounds = true*/)};
}

auto BinopPtrRightAdd(llvm::Type *lty, llvm::Value *left, llvm::Type *rty,
                      llvm::Value *right, const Environment &env)
    -> Outcome<llvm::Value *, Error> {
  /*
   *  This is the same behavior as BinopPtrLeftAdd, except that we assume
   *  the integer is the left term and the pointer is the right term.
   *
   */
  assert(lty != nullptr);
  assert(rty != nullptr);
  assert(left != nullptr);
  assert(right != nullptr);

  std::vector<llvm::Value *> indicies = {left};
  return {env.instruction_builder->CreateGEP(rty, right, indicies,
                                             "gep" /* isInBounds = true*/)};
}

void InitializeBinopPrimitives(const Environment &env) {
  /*
      precedence table:
          ==        : 1
          < <= > >= : 2
          & |       : 3
          + -       : 4
          * / %     : 5
  */
  InternedString minus = env.operators->Intern("-");
  InternedString plus = env.operators->Intern("+");
  InternedString mul = env.operators->Intern("*");
  InternedString div = env.operators->Intern("/");
  InternedString mod = env.operators->Intern("%");
  InternedString land = env.operators->Intern("&");
  InternedString lor = env.operators->Intern("|");
  InternedString cmpeq = env.operators->Intern("==");
  InternedString cmpne = env.operators->Intern("!=");
  InternedString cmplt = env.operators->Intern("<");
  InternedString cmple = env.operators->Intern("<=");
  InternedString cmpgt = env.operators->Intern(">");
  InternedString cmpge = env.operators->Intern(">=");

  Type *int_ty = env.types->GetIntType();
  Type *bool_ty = env.types->GetBoolType();
  Type *int_ptr_ty = env.types->GetPointerType(int_ty);
  Type *bool_ptr_ty = env.types->GetPointerType(bool_ty);

  Associativity left_assoc = Associativity::Left;
  // Associativity right_assoc = Associativity::Right;
  Precedence one = 1;
  Precedence two = 2;
  Precedence three = 3;
  Precedence four = 4;
  Precedence five =
      5; // NOLINT; we are defining the 'magic' precedence number on this line.

  env.binops->Register(cmpeq, one, left_assoc, int_ty, int_ty, bool_ty,
                       BinopIntEq);
  env.binops->Register(cmpeq, one, left_assoc, bool_ty, bool_ty, bool_ty,
                       BinopBoolEq);
  env.binops->Register(cmpne, two, left_assoc, int_ty, int_ty, bool_ty,
                       BinopIntNe);
  env.binops->Register(cmpne, two, left_assoc, bool_ty, bool_ty, bool_ty,
                       BinopBoolNe);
  env.binops->Register(cmplt, two, left_assoc, int_ty, int_ty, bool_ty,
                       BinopIntLt);
  env.binops->Register(cmple, two, left_assoc, int_ty, int_ty, bool_ty,
                       BinopIntLe);
  env.binops->Register(cmpgt, two, left_assoc, int_ty, int_ty, bool_ty,
                       BinopIntGt);
  env.binops->Register(cmpge, two, left_assoc, int_ty, int_ty, bool_ty,
                       BinopIntGe);
  env.binops->Register(land, three, left_assoc, bool_ty, bool_ty, bool_ty,
                       BinopBoolAnd);
  env.binops->Register(lor, three, left_assoc, bool_ty, bool_ty, bool_ty,
                       BinopBoolOr);
  env.binops->Register(plus, four, left_assoc, int_ty, int_ty, int_ty,
                       BinopIntAdd);
  env.binops->Register(minus, four, left_assoc, int_ty, int_ty, int_ty,
                       BinopIntSub);
  env.binops->Register(mul, five, left_assoc, int_ty, int_ty, int_ty,
                       BinopIntMul);
  env.binops->Register(div, five, left_assoc, int_ty, int_ty, int_ty,
                       BinopIntSDiv);
  env.binops->Register(mod, five, left_assoc, int_ty, int_ty, int_ty,
                       BinopIntMod);

  // these are staying for now, the solution we have gone with is to simply
  // define a new instance of the pointer arithmetic binop for each type we
  // perform pointer arithmetic upon. this is inefficient, but it works, because
  // the GEP instruction works on any type of pointer. #NOTE: this is prime
  // optimization material.. OH! we can optimize this by simply looking up this
  // value in the table when we detect this case, and then simply using the
  // codegen fn associated with this function which we already looked up, and we
  // know it works regardless of type. the types present in the function calls
  // within this init routine are for normal lookup against regular unops, like
  // integer negation. not these 'polymorphic' procedures. (they aren't truly
  // 'polymorphic' in that sense, but they do work for any type T)
  env.binops->Register(plus, four, left_assoc, int_ptr_ty, int_ty, int_ptr_ty,
                       BinopPtrLeftAdd);
  env.binops->Register(plus, four, left_assoc, int_ty, int_ptr_ty, int_ptr_ty,
                       BinopPtrRightAdd);
  env.binops->Register(plus, four, left_assoc, bool_ptr_ty, int_ty, bool_ptr_ty,
                       BinopPtrLeftAdd);
  env.binops->Register(plus, four, left_assoc, int_ty, bool_ptr_ty, bool_ptr_ty,
                       BinopPtrRightAdd);
}
} // namespace pink
