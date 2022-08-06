
#include "kernel/UnopPrimitives.h"

#include "ops/UnopTable.h"

namespace pink {
    Outcome<llvm::Value*, Error> UnopIntNegate(llvm::Value* term, std::shared_ptr<Environment> env)
    {
    	llvm::Value* value = env->builder->CreateNeg(term, "neg");
      Outcome<llvm::Value*, Error> result(value);
      return result;
    }

    Outcome<llvm::Value*, Error> UnopBoolNegate(llvm::Value* term, std::shared_ptr<Environment> env)
    {
    	llvm::Value* value = env->builder->CreateNot(term, "not");
      Outcome<llvm::Value*, Error> result(value);
      return result;
    }

    Outcome<llvm::Value*, Error> UnopAddressOfValue(llvm::Value* term, std::shared_ptr<Environment> env)
    {
      return Outcome<llvm::Value*, Error>(term);
    }

    Outcome<llvm::Value*, Error> UnopValueOfAddress(llvm::Value* term, std::shared_ptr<Environment> env)
    {
      return Outcome<llvm::Value*, Error>(term);
    }


    void InitializeUnopPrimitives(std::shared_ptr<Environment> env)
    {
        Type* int_ty = env->types->GetIntType();
        Type* bool_ty = env->types->GetBoolType();
        Type* int_ptr_ty = env->types->GetPointerType(int_ty);
        Type* bool_ptr_ty = env->types->GetPointerType(bool_ty);

        InternedString neg_str = env->operators->Intern("-");
        InternedString not_str = env->operators->Intern("!");
        InternedString AOV_str = env->operators->Intern("&");
        InternedString VOA_str = env->operators->Intern("*");

        env->unops->Register(neg_str, int_ty, int_ty, UnopIntNegate);
        env->unops->Register(not_str, bool_ty, bool_ty, UnopBoolNegate);

        // these operators are unique from the others in that the way we 
        // take the address of some memory location is independant of 
        // the type of that particular memory location. meaning the
        // implementation for either type can be the exact same underlying
        // code. This is partially because pointers are all the same size, 
        // and are at the hardware level indistiguishable from one another
        // by their types. it is only the compiler itself which knows the 
        // type of any particular pointer, and the hardware is only concerned 
        // with the address the pointer contains.
        env->unops->Register(AOV_str, int_ty, int_ptr_ty, UnopAddressOfValue);
        env->unops->Register(AOV_str, bool_ty, bool_ptr_ty, UnopAddressOfValue);
        // and the same is true of the reverse operation.
        env->unops->Register(VOA_str, int_ptr_ty, int_ty, UnopValueOfAddress);
        env->unops->Register(VOA_str, bool_ptr_ty, bool_ty, UnopValueOfAddress);

    }
}
