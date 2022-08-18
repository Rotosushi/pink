
#include "kernel/DefaultInitialization.h"

namespace pink {
  
  llvm::Constant* GetDefaultInitialization(llvm::Type* type, std::shared_ptr<Environment> env)
  {
    if (llvm::IntegerType* it = llvm::dyn_cast<llvm::IntegerType>(type))
    {
      return env->builder->getIntN(it->getBitWidth(), 0);      
    }
    else if (llvm::ArrayType* at = llvm::dyn_cast<llvm::ArrayType>(type))
    {
      std::vector<llvm::Constant*> members;

      size_t      length = at->getNumElements();
      llvm::Type* elemTy = at->getElementType();

      for (size_t i = 0; i < length; i++)
      {
        members.emplace_back(GetDefaultInitialization(elemTy, env));
      }

      return llvm::ConstantArray::get(at, members);
    }
    else if (llvm::PointerType* pt = llvm::dyn_cast<llvm::PointerType>(type))
    {
      return llvm::ConstantPointerNull::get(pt);
    }
    else
    {
      FatalError("Given a unsupported llvm::Type* to GetDefaultInitialization", __FILE__, __LINE__);
      return (llvm::Constant*)nullptr;
    }
  }


}



