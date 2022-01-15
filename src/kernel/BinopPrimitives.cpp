#include "kernel/BinopPrimitives.h"
#include "ops/BinopTable.h"

namespace pink {
    Outcome<llvm::Value*, Error> BinopIntAdd(llvm::Value* left, llvm::Value* right, Environment& env)
    {
        Outcome<llvm::Value*, Error> result(env.ir_builder.CreateAdd(left, right, "iadd"));
        return result;
    }

    Outcome<llvm::Value*, Error> BinopIntSub(llvm::Value* left, llvm::Value* right, Environment& env)
    {
        Outcome<llvm::Value*, Error> result(env.ir_builder.CreateSub(left, right, "isub"));
        return result;
    }

    Outcome<llvm::Value*, Error> BinopIntMul(llvm::Value* left, llvm::Value* right, Environment& env)
    {
        Outcome<llvm::Value*, Error> result(env.ir_builder.CreateMul(left, right, "imul"));
        return result;
    }

    Outcome<llvm::Value*, Error> BinopIntSDiv(llvm::Value* left, llvm::Value* right, Environment& env)
    {
        Outcome<llvm::Value*, Error> result(env.ir_builder.CreateSDiv(left, right, "idiv"));
        return result;
    }

    Outcome<llvm::Value*, Error> BinopIntMod(llvm::Value* left, llvm::Value* right, Environment& env)
    {
        Outcome<llvm::Value*, Error> result(env.ir_builder.CreateSRem(left, right, "imod"));
        return result;
    }

    Outcome<llvm::Value*, Error> BinopIntEq(llvm::Value* left, llvm::Value* right, Environment& env)
    {
        Outcome<llvm::Value*, Error> result(env.ir_builder.CreateICmpEQ(left, right, "idiv"));
        return result;
    }

    Outcome<llvm::Value*, Error> BinopBoolEq(llvm::Value* left, llvm::Value* right, Environment& env)
    {
        Outcome<llvm::Value*, Error> result(env.ir_builder.CreateICmpEQ(left, right, "idiv"));
        return result;
    }

    Outcome<llvm::Value*, Error> BinopBoolAnd(llvm::Value* left, llvm::Value* right, Environment& env)
    {
        Outcome<llvm::Value*, Error> result(env.ir_builder.CreateLogicalAnd(left, right, "land"));
        return result;
    }

    Outcome<llvm::Value*, Error> BinopBoolOr(llvm::Value* left, llvm::Value* right, Environment& env)
    {
        Outcome<llvm::Value*, Error> result(env.ir_builder.CreateLogicalOr(left, right, "lor"));
        return result;
    }

    void InitializeBinopPrimitives(Environment& env)
    {
        /*
            precedence table:
                ==    : 1
                && || : 2
                + -   : 3
                * \ % : 4
        */
        InternedString minus = env.operators.Intern("-");
        InternedString plus  = env.operators.Intern("+");
        InternedString mul   = env.operators.Intern("*");
        InternedString div   = env.operators.Intern("\\");
        InternedString mod   = env.operators.Intern("%");
        InternedString land  = env.operators.Intern("&&");
        InternedString lor   = env.operators.Intern("||");
        InternedString cmpeq = env.operators.Intern("==");

        Type* int_ty  = env.types.GetIntType();
        Type* bool_ty = env.types.GetBoolType();

        Associativity left_assoc = Associativity::Left;

        env.binops.Register(cmpeq, 1, left_assoc, int_ty, int_ty, int_ty, BinopIntEq);
        env.binops.Register(cmpeq, 1, left_assoc, bool_ty, bool_ty, bool_ty, BinopBoolEq);
        env.binops.Register(land,  2, left_assoc, bool_ty, bool_ty, bool_ty, BinopBoolAnd);
        env.binops.Register(lor,   2, left_assoc, bool_ty, bool_ty, bool_ty, BinopBoolOr);
        env.binops.Register(plus,  3, left_assoc, int_ty, int_ty, int_ty, BinopIntAdd);
        env.binops.Register(minus, 3, left_assoc, int_ty, int_ty, int_ty, BinopIntSub);
        env.binops.Register(mul,   4, left_assoc, int_ty, int_ty, int_ty, BinopIntMul);
        env.binops.Register(div,   4, left_assoc, int_ty, int_ty, int_ty, BinopIntSDiv);
        env.binops.Register(mod,   4, left_assoc, int_ty, int_ty, int_ty, BinopIntMod);

    }
}
