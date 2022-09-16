#pragma once 

#include "aux/Outcome.h"
#include "aux/Environment.h"

#include "llvm/IR/Value.h"
#include "llvm/IR/Type.h"


namespace pink{
  Outcome<llvm::Value*, Error> Cast(llvm::Value* value, llvm::Type* target_type, const Environment& env);
}



