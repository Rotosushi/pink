#include "kernel/ops/BinopPrimitives.h"
#include "ops/BinopTable.h"

namespace pink {
auto BinopIntAdd(llvm::Type *lty, llvm::Value *left, llvm::Type *rty,
                 llvm::Value *right, const Environment &env) -> llvm::Value * {
  assert(lty != nullptr);
  assert(rty != nullptr);
  assert(left != nullptr);
  assert(right != nullptr);

  return env.instruction_builder->CreateAdd(left, right, "iadd");
}

auto BinopIntSub(llvm::Type *lty, llvm::Value *left, llvm::Type *rty,
                 llvm::Value *right, const Environment &env) -> llvm::Value * {
  assert(lty != nullptr);
  assert(rty != nullptr);
  assert(left != nullptr);
  assert(right != nullptr);

  return env.instruction_builder->CreateSub(left, right, "isub");
}

auto BinopIntMul(llvm::Type *lty, llvm::Value *left, llvm::Type *rty,
                 llvm::Value *right, const Environment &env) -> llvm::Value * {
  assert(lty != nullptr);
  assert(rty != nullptr);
  assert(left != nullptr);
  assert(right != nullptr);

  return env.instruction_builder->CreateMul(left, right, "imul");
}

auto BinopIntSDiv(llvm::Type *lty, llvm::Value *left, llvm::Type *rty,
                  llvm::Value *right, const Environment &env) -> llvm::Value * {
  assert(lty != nullptr);
  assert(rty != nullptr);
  assert(left != nullptr);
  assert(right != nullptr);

  return env.instruction_builder->CreateSDiv(left, right, "idiv");
}

auto BinopIntMod(llvm::Type *lty, llvm::Value *left, llvm::Type *rty,
                 llvm::Value *right, const Environment &env) -> llvm::Value * {
  assert(lty != nullptr);
  assert(rty != nullptr);
  assert(left != nullptr);
  assert(right != nullptr);

  return env.instruction_builder->CreateSRem(left, right, "imod");
}

auto BinopIntEq(llvm::Type *lty, llvm::Value *left, llvm::Type *rty,
                llvm::Value *right, const Environment &env) -> llvm::Value * {
  assert(lty != nullptr);
  assert(rty != nullptr);
  assert(left != nullptr);
  assert(right != nullptr);

  return env.instruction_builder->CreateICmpEQ(left, right, "ieq");
}

auto BinopBoolEq(llvm::Type *lty, llvm::Value *left, llvm::Type *rty,
                 llvm::Value *right, const Environment &env) -> llvm::Value * {
  assert(lty != nullptr);
  assert(rty != nullptr);
  assert(left != nullptr);
  assert(right != nullptr);

  return env.instruction_builder->CreateICmpEQ(left, right, "booleq");
}

auto BinopIntNe(llvm::Type *lty, llvm::Value *left, llvm::Type *rty,
                llvm::Value *right, const Environment &env) -> llvm::Value * {
  assert(lty != nullptr);
  assert(rty != nullptr);
  assert(left != nullptr);
  assert(right != nullptr);

  return env.instruction_builder->CreateICmpNE(left, right, "ineq");
}

auto BinopBoolNe(llvm::Type *lty, llvm::Value *left, llvm::Type *rty,
                 llvm::Value *right, const Environment &env) -> llvm::Value * {
  assert(lty != nullptr);
  assert(rty != nullptr);
  assert(left != nullptr);
  assert(right != nullptr);

  return env.instruction_builder->CreateICmpNE(left, right, "boolneq");
}

auto BinopIntGt(llvm::Type *lty, llvm::Value *left, llvm::Type *rty,
                llvm::Value *right, const Environment &env) -> llvm::Value * {
  assert(lty != nullptr);
  assert(rty != nullptr);
  assert(left != nullptr);
  assert(right != nullptr);

  return env.instruction_builder->CreateICmpSGT(left, right, "igt");
}

auto BinopIntGe(llvm::Type *lty, llvm::Value *left, llvm::Type *rty,
                llvm::Value *right, const Environment &env) -> llvm::Value * {
  assert(lty != nullptr);
  assert(rty != nullptr);
  assert(left != nullptr);
  assert(right != nullptr);

  return env.instruction_builder->CreateICmpSGE(left, right, "ige");
}

auto BinopIntLt(llvm::Type *lty, llvm::Value *left, llvm::Type *rty,
                llvm::Value *right, const Environment &env) -> llvm::Value * {
  assert(lty != nullptr);
  assert(rty != nullptr);
  assert(left != nullptr);
  assert(right != nullptr);

  return env.instruction_builder->CreateICmpSLT(left, right, "ilt");
}

auto BinopIntLe(llvm::Type *lty, llvm::Value *left, llvm::Type *rty,
                llvm::Value *right, const Environment &env) -> llvm::Value * {
  assert(lty != nullptr);
  assert(rty != nullptr);
  assert(left != nullptr);
  assert(right != nullptr);

  return env.instruction_builder->CreateICmpSLE(left, right, "ile");
}

auto BinopBoolAnd(llvm::Type *lty, llvm::Value *left, llvm::Type *rty,
                  llvm::Value *right, const Environment &env) -> llvm::Value * {
  assert(lty != nullptr);
  assert(rty != nullptr);
  assert(left != nullptr);
  assert(right != nullptr);

  return env.instruction_builder->CreateAnd(left, right, "and");
}

auto BinopBoolOr(llvm::Type *lty, llvm::Value *left, llvm::Type *rty,
                 llvm::Value *right, const Environment &env) -> llvm::Value * {
  assert(lty != nullptr);
  assert(rty != nullptr);
  assert(left != nullptr);
  assert(right != nullptr);

  return env.instruction_builder->CreateOr(left, right, "or");
}

/*
auto BinopSliceLeftAdd(llvm::Type *lty, llvm::Value *left, llvm::Type *rty,
                       llvm::Value *right, const Environment &env)
    -> llvm::Value * {
  assert(lty != nullptr);
  assert(rty != nullptr);
  assert(left != nullptr);
  assert(right != nullptr);

  std::vector<llvm::Value *> indicies = {right};
  return {env.instruction_builder->CreateGEP(lty, left, indicies,
                                             "gep")};
}

auto BinopSliceRightAdd(llvm::Type *lty, llvm::Value *left, llvm::Type *rty,
                        llvm::Value *right, const Environment &env)
    -> Outcome<llvm::Value *, Error> {
  assert(lty != nullptr);
  assert(rty != nullptr);
  assert(left != nullptr);
  assert(right != nullptr);

  std::vector<llvm::Value *> indicies = {left};
  return {env.instruction_builder->CreateGEP(rty, right, indicies,
                                             "gep")};
}
*/

void InitializeBinopPrimitives(const Environment &env) {
  /*
      precedence table:
          ==        : 1
          < <= > >= : 2
          & |       : 3
          + -       : 4
          * / %     : 5
  */
  InternedString minus     = env.operators->Intern("-");
  InternedString plus      = env.operators->Intern("+");
  InternedString mul       = env.operators->Intern("*");
  InternedString div       = env.operators->Intern("/");
  InternedString mod       = env.operators->Intern("%");
  InternedString land      = env.operators->Intern("&");
  InternedString lor       = env.operators->Intern("|");
  InternedString cmpeq     = env.operators->Intern("==");
  InternedString cmpne     = env.operators->Intern("!=");
  InternedString cmplt     = env.operators->Intern("<");
  InternedString cmple     = env.operators->Intern("<=");
  InternedString cmpgt     = env.operators->Intern(">");
  InternedString cmpge     = env.operators->Intern(">=");

  Type::Pointer int_ty     = env.types->GetIntType();
  Type::Pointer bool_ty    = env.types->GetBoolType();
  // Type *int_slice_ty       = env.types->GetSliceType(int_ty);
  // Type *bool_slice_ty      = env.types->GetSliceType(bool_ty);

  Associativity left_assoc = Associativity::Left;
  // Associativity right_assoc = Associativity::Right;
  Precedence    one        = 1;
  Precedence    two        = 2;
  Precedence    three      = 3;
  Precedence    four       = 4;
  Precedence    five       = 5;

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

  /*
    env.binops->Register(plus, four, left_assoc, int_slice_ty, int_ty,
                         int_slice_ty, BinopSliceLeftAdd);
    env.binops->Register(plus, four, left_assoc, int_ty, int_slice_ty,
                         int_slice_ty, BinopSliceRightAdd);
    env.binops->Register(plus, four, left_assoc, bool_slice_ty, int_ty,
                         bool_slice_ty, BinopSliceLeftAdd);
    env.binops->Register(plus, four, left_assoc, int_ty, bool_slice_ty,
                         bool_slice_ty, BinopSliceRightAdd);
  */
}
} // namespace pink
