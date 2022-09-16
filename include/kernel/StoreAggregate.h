#pragma once 
#include "aux/Environment.h"

#include "llvm/IR/Type.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Constant.h"


namespace pink {
  void StoreConstAggregate(llvm::Type* ty, llvm::Value* dest, llvm::Constant* src, const Environment& env);
  void StoreValueAggregate(llvm::Type* ty, llvm::Value* dest, llvm::Value* src, const Environment& env);
  void StoreAggregate(llvm::Type* ty, llvm::Value* dest, llvm::Value* src, const Environment& env);

}


/*------------------*/
