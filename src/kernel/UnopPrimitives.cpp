
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
      // we want a unop expression which returns a llvm::Value* which represents the
      // llvm::Value* which was received as a pointer to that very thing.
      // luckily, whenever we allocate memory (global or stack) within llvm,
      // the llvm::Value* which is returned is -already- representing the 
      // address of the new allocation. This means that any llvm::Value* we 
      // receive which is a llvm::AllocaInst or a llvm::GlobalVariable we can 
      // simply return. however, due to the type of the unop '&' associated 
      // with this procedure, the type of this llvm::Value has been changed to 
      // be a PointerType, meaning we can handle it differently at a later
      // point. 
      // additionally, and for the same reasons, this works for the reverse
      // operation as well. because all the places which we emit loads/stores 
      // for care about is the type we are loading/storing, and the value being 
      // stored. when we attempt to store a value at some location if we say 
      // to store a value of some concrete type to the given pointer, we can 
      // simply have that concrete value be a pointer itself, and the type 
      // being store be a pointer to that concrete type.
      //
      // we know that any pointer we create must be either from 
      // llvm::IRBuilder<>::CreateAlloca, or llvm::Module::getOrInsertGlobal
      // thus we only need to check that the value we are 'taking the address of'
      // is one of this llvm classes. 
      // #TODO: currently we don't have a way of reporting the error's location 
      // from within this function! normally we are either parsing, and thus know
      // the location, or we are typing/codegening an Ast, and thus have a copy of the
      // location to reference. But here we are only dealing directly with the
      // llvm::Value* itself, which does not record the location. 
      // (nor should it lol)
      // Yet it sure would be nice to have the error reporting procedure
      // correctly point out the location of the bad unop within the term 
      // that produced the error.
      if (!(llvm::isa<llvm::AllocaInst>(term) || !(llvm::isa<llvm::GlobalVariable>(term))))
      {
        Error error(Error::Code::CannotTakeAddressOfLiteral, Location());
        return Outcome<llvm::Value*, Error>(error);  
      }

      return Outcome<llvm::Value*, Error>(term);
    }

    Outcome<llvm::Value*, Error> UnopValueOfAddress(llvm::Value* term, std::shared_ptr<Environment> env)
    {
      if (!(llvm::isa<llvm::AllocaInst>(term)) || !(llvm::isa<llvm::GlobalVariable>(term)))
      {
        Error error(Error::Code::CannotDereferenceLiteral, Location());
        return Outcome<llvm::Value*, Error>(error);
      }

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
