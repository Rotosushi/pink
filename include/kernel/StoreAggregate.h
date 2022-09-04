#pragma once 
#include "aux/Environment.h"

#include "llvm/IR/Type.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Constant.h"


namespace pink {
  llvm::Value* StoreConstAggregate(llvm::Type* ty, llvm::Value* dest, llvm::Constant* src, std::shared_ptr<Environment> env);
  llvm::Value* StoreValueAggregate(llvm::Type* ty, llvm::Value* dest, llvm::Value* src, std::shared_ptr<Environment> env);
  llvm::Value* StoreAggregate(llvm::Type* ty, llvm::Value* dest, llvm::Value* src, std::shared_ptr<Environment> env);

}


/*------------------*/
