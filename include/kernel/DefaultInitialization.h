#pragma once 
#include "aux/Outcome.h"
#include "aux/Environment.h"

#include "llvm/IR/Type.h"
#include "llvm/IR/Constant.h"

namespace pink {
  // We don't return an Outcome here because I cannot think of a 
  // situation where we give this function a type that is not 
  // supported for default initialization where that is not the 
  // compilers fault. Because we can catch that error much earlier 
  // than this function call. such as during typechecking.
  // so instead, if this function is given a type it cannot 
  // translate into a default value, it raises this situation 
  // to a FatalError.
  llvm::Constant* GetDefaultInitialization(llvm::Type* type, std::shared_ptr<Environment> env);


}
