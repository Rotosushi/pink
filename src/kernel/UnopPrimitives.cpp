
#include "kernel/UnopPrimitives.h"

#include "ops/UnopTable.h"

namespace pink {
    Outcome<llvm::Value*, Error> UnopIntNegate(llvm::Value* term, Environment& env)
    {
        Outcome<llvm::Value*, Error> result(env.ir_builder.CreateNeg(term, "neg"));
        return result;
    }

    Outcome<llvm::Value*, Error> UnopBoolNegate(llvm::Value* term, Environment& env)
    {
        Outcome<llvm::Value*, Error> result(env.ir_builder.CreateNot(term, "not"));
        return result;
    }

    void InitializeUnopPrimitives(Environment& env)
    {
        Type* int_ty = env.types.GetIntType();
        InternedString neg_str = env.operators.Intern("-");
        env.unops.Register(neg_str, int_ty, int_ty, UnopIntNegate);

        Type* bool_ty = env.types.GetBoolType();
        InternedString not_str = env.operators.Intern("!");
        env.unops.Register(not_str, bool_ty, bool_ty, UnopBoolNegate);

    }
}
