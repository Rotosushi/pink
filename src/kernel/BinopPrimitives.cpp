#include "kernel/BinopPrimitives.h"
#include "ops/BinopTable.h"

namespace pink {
    Outcome<llvm::Value*, Error> BinopIntAdd(llvm::Value* left, llvm::Value* right, std::shared_ptr<Environment> env)
    {
    	llvm::Value* value = env->builder->CreateAdd(left, right, "iadd");
        Outcome<llvm::Value*, Error> result(value);
        return result;
    }

    Outcome<llvm::Value*, Error> BinopIntSub(llvm::Value* left, llvm::Value* right, std::shared_ptr<Environment> env)
    {
    	llvm::Value* value = env->builder->CreateSub(left, right, "isub");
        Outcome<llvm::Value*, Error> result(value);
        return result;
    }

    Outcome<llvm::Value*, Error> BinopIntMul(llvm::Value* left, llvm::Value* right, std::shared_ptr<Environment> env)
    {
    	llvm::Value* value = env->builder->CreateMul(left, right, "imul");
        Outcome<llvm::Value*, Error> result(value);
        return result;
    }

    Outcome<llvm::Value*, Error> BinopIntSDiv(llvm::Value* left, llvm::Value* right, std::shared_ptr<Environment> env)
    {
    	llvm::Value* value = env->builder->CreateSDiv(left, right, "idiv");
        Outcome<llvm::Value*, Error> result(value);
        return result;
    }

    Outcome<llvm::Value*, Error> BinopIntMod(llvm::Value* left, llvm::Value* right, std::shared_ptr<Environment> env)
    {
    	llvm::Value* value = env->builder->CreateSRem(left, right, "imod");
        Outcome<llvm::Value*, Error> result(value);
        return result;
    }

    Outcome<llvm::Value*, Error> BinopIntEq(llvm::Value* left, llvm::Value* right, std::shared_ptr<Environment> env)
    {
    	llvm::Value* value = env->builder->CreateICmpEQ(left, right, "ieq");
        Outcome<llvm::Value*, Error> result(value);
        return result;
    }

    Outcome<llvm::Value*, Error> BinopBoolEq(llvm::Value* left, llvm::Value* right, std::shared_ptr<Environment> env)
    {
    	llvm::Value* value = env->builder->CreateICmpEQ(left, right, "booleq");
        Outcome<llvm::Value*, Error> result(value);
        return result;
    }

    Outcome<llvm::Value*, Error> BinopBoolAnd(llvm::Value* left, llvm::Value* right, std::shared_ptr<Environment> env)
    {
    	llvm::Value* value = env->builder->CreateAnd(left, right, "and");
        Outcome<llvm::Value*, Error> result(value);
        return result;
    }

    Outcome<llvm::Value*, Error> BinopBoolOr(llvm::Value* left, llvm::Value* right, std::shared_ptr<Environment> env)
    {
    	llvm::Value* value = env->builder->CreateOr(left, right, "or");
        Outcome<llvm::Value*, Error> result(value);
        return result;
    }

    void InitializeBinopPrimitives(std::shared_ptr<Environment> env)
    {
        /*
            precedence table:
                ==    : 1
                && || : 2
                + -   : 3
                * / % : 4
        */
        InternedString minus = env->operators->Intern("-");
        InternedString plus  = env->operators->Intern("+");
        InternedString mul   = env->operators->Intern("*");
        InternedString div   = env->operators->Intern("/");
        InternedString mod   = env->operators->Intern("%");
        InternedString land  = env->operators->Intern("&");
        InternedString lor   = env->operators->Intern("|");
        InternedString cmpeq = env->operators->Intern("==");

        Type* int_ty  = env->types->GetIntType();
        Type* bool_ty = env->types->GetBoolType();

        Associativity left_assoc = Associativity::Left;

        env->binops->Register(cmpeq, 1, left_assoc, int_ty, int_ty, bool_ty, BinopIntEq);
        env->binops->Register(cmpeq, 1, left_assoc, bool_ty, bool_ty, bool_ty, BinopBoolEq);
        env->binops->Register(land,  2, left_assoc, bool_ty, bool_ty, bool_ty, BinopBoolAnd);
        env->binops->Register(lor,   2, left_assoc, bool_ty, bool_ty, bool_ty, BinopBoolOr);
        env->binops->Register(plus,  3, left_assoc, int_ty, int_ty, int_ty, BinopIntAdd);
        env->binops->Register(minus, 3, left_assoc, int_ty, int_ty, int_ty, BinopIntSub);
        env->binops->Register(mul,   4, left_assoc, int_ty, int_ty, int_ty, BinopIntMul);
        env->binops->Register(div,   4, left_assoc, int_ty, int_ty, int_ty, BinopIntSDiv);
        env->binops->Register(mod,   4, left_assoc, int_ty, int_ty, int_ty, BinopIntMod);

    }
}
