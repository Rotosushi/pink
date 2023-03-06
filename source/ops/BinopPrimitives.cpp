#include "aux/Environment.h"

namespace pink {
auto BinopIntAdd(llvm::Value *left, llvm::Value *right, Environment &env)
    -> llvm::Value * {
  return env.CreateAdd(left, right, "iadd");
}

auto BinopIntSub(llvm::Value *left, llvm::Value *right, Environment &env)
    -> llvm::Value * {
  return env.CreateSub(left, right, "isub");
}

auto BinopIntMul(llvm::Value *left, llvm::Value *right, Environment &env)
    -> llvm::Value * {
  return env.CreateMul(left, right, "imul");
}

auto BinopIntSDiv(llvm::Value *left, llvm::Value *right, Environment &env)
    -> llvm::Value * {
  return env.CreateSDiv(left, right, "idiv");
}

auto BinopIntMod(llvm::Value *left, llvm::Value *right, Environment &env)
    -> llvm::Value * {
  return env.CreateSRem(left, right, "imod");
}

auto BinopIntEq(llvm::Value *left, llvm::Value *right, Environment &env)
    -> llvm::Value * {
  return env.CreateICmpEQ(left, right, "ieq");
}

auto BinopBoolEq(llvm::Value *left, llvm::Value *right, Environment &env)
    -> llvm::Value * {
  return env.CreateICmpEQ(left, right, "ieq");
}

auto BinopIntNe(llvm::Value *left, llvm::Value *right, Environment &env)
    -> llvm::Value * {
  return env.CreateICmpNE(left, right, "ineq");
}

auto BinopBoolNe(llvm::Value *left, llvm::Value *right, Environment &env)
    -> llvm::Value * {
  return env.CreateICmpNE(left, right, "boolneq");
}

auto BinopIntGt(llvm::Value *left, llvm::Value *right, Environment &env)
    -> llvm::Value * {
  return env.CreateICmpSGT(left, right, "igt");
}

auto BinopIntGe(llvm::Value *left, llvm::Value *right, Environment &env)
    -> llvm::Value * {
  return env.CreateICmpSGE(left, right, "ige");
}

auto BinopIntLt(llvm::Value *left, llvm::Value *right, Environment &env)
    -> llvm::Value * {
  return env.CreateICmpSLT(left, right, "ilt");
}

auto BinopIntLe(llvm::Value *left, llvm::Value *right, Environment &env)
    -> llvm::Value * {
  return env.CreateICmpSLE(left, right, "ile");
}

auto BinopBoolAnd(llvm::Value *left, llvm::Value *right, Environment &env)
    -> llvm::Value * {
  return env.CreateAnd(left, right, "and");
}

auto BinopBoolOr(llvm::Value *left, llvm::Value *right, Environment &env)
    -> llvm::Value * {
  return env.CreateOr(left, right, "or");
}

void InitializeBinopPrimitives(Environment &env) {
  InternedString minus = env.InternOperator("-");
  InternedString plus  = env.InternOperator("+");
  InternedString mul   = env.InternOperator("*");
  InternedString div   = env.InternOperator("/");
  InternedString mod   = env.InternOperator("%");
  InternedString land  = env.InternOperator("&");
  InternedString lor   = env.InternOperator("|");
  InternedString cmpeq = env.InternOperator("==");
  InternedString cmpne = env.InternOperator("!=");
  InternedString cmplt = env.InternOperator("<");
  InternedString cmple = env.InternOperator("<=");
  InternedString cmpgt = env.InternOperator(">");
  InternedString cmpge = env.InternOperator(">=");

  TypeInterface::Pointer int_ty  = env.GetIntType();
  TypeInterface::Pointer bool_ty = env.GetBoolType();

  Associativity left_assoc = Associativity::Left;
  // Associativity right_assoc = Associativity::Right;

  /*
    precedence table:
        ==        : 1
        < <= > >= : 2
        & |       : 3
        + -       : 4
        * / %     : 5
*/
  Precedence const one   = 1;
  Precedence const two   = 2;
  Precedence const three = 3;
  Precedence const four  = 4;
  Precedence const five  = 5;

  env.RegisterBinop(cmpeq,
                    one,
                    left_assoc,
                    int_ty,
                    int_ty,
                    bool_ty,
                    BinopIntEq);
  env.RegisterBinop(cmpeq,
                    one,
                    left_assoc,
                    bool_ty,
                    bool_ty,
                    bool_ty,
                    BinopBoolEq);
  env.RegisterBinop(cmpne,
                    two,
                    left_assoc,
                    int_ty,
                    int_ty,
                    bool_ty,
                    BinopIntNe);
  env.RegisterBinop(cmpne,
                    two,
                    left_assoc,
                    bool_ty,
                    bool_ty,
                    bool_ty,
                    BinopBoolNe);
  env.RegisterBinop(cmplt,
                    two,
                    left_assoc,
                    int_ty,
                    int_ty,
                    bool_ty,
                    BinopIntLt);
  env.RegisterBinop(cmple,
                    two,
                    left_assoc,
                    int_ty,
                    int_ty,
                    bool_ty,
                    BinopIntLe);
  env.RegisterBinop(cmpgt,
                    two,
                    left_assoc,
                    int_ty,
                    int_ty,
                    bool_ty,
                    BinopIntGt);
  env.RegisterBinop(cmpge,
                    two,
                    left_assoc,
                    int_ty,
                    int_ty,
                    bool_ty,
                    BinopIntGe);
  env.RegisterBinop(land,
                    three,
                    left_assoc,
                    bool_ty,
                    bool_ty,
                    bool_ty,
                    BinopBoolAnd);
  env.RegisterBinop(lor,
                    three,
                    left_assoc,
                    bool_ty,
                    bool_ty,
                    bool_ty,
                    BinopBoolOr);
  env.RegisterBinop(plus,
                    four,
                    left_assoc,
                    int_ty,
                    int_ty,
                    int_ty,
                    BinopIntAdd);
  env.RegisterBinop(minus,
                    four,
                    left_assoc,
                    int_ty,
                    int_ty,
                    int_ty,
                    BinopIntSub);
  env.RegisterBinop(mul, five, left_assoc, int_ty, int_ty, int_ty, BinopIntMul);
  env.RegisterBinop(div,
                    five,
                    left_assoc,
                    int_ty,
                    int_ty,
                    int_ty,
                    BinopIntSDiv);
  env.RegisterBinop(mod, five, left_assoc, int_ty, int_ty, int_ty, BinopIntMod);
}
} // namespace pink
