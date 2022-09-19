/**
 * @file StoreAggregate.h
 * @brief Header for StoreAggregate
 * @version 0.1
 */
#pragma once 
#include "aux/Environment.h"

#include "llvm/IR/Type.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Constant.h"


namespace pink {
  /**
   * @brief Store src into dest
   * 
   * this function handles the case when a member of the given aggregate is 
   * itself an aggregate.
   * 
   * @param ty the type being stored
   * @param dest pointer to the destination. must be a valid pointer to memory large enough to store ty
   * @param src pointer to the source. must be a pointer to a llvm::Constant of type ty, already initialized
   * @param env The environment of this compilation unit
   */
  void StoreConstAggregate(llvm::Type* ty, llvm::Value* dest, llvm::Constant* src, const Environment& env);
  
  /**
   * @brief Store src into dest
   * 
   * @param ty the type being stored
   * @param dest pointer to the destination. must be a valid pointer to memory large enough to store ty
   * @param src pointer to the source. must be a pointer to valid memory large enough to store ty, and already initialized
   * @param env the environment of this compilation unit
   */
  void StoreValueAggregate(llvm::Type* ty, llvm::Value* dest, llvm::Value* src, const Environment& env);
  
  /**
   * @brief Store src into dest
   * 
   * this function inserts any load/store instructions wherever the IRBuilder member of the 
   * env is currently pointing.
   * 
   * @param ty the type being stored
   * @param dest pointer to the destination. must be a valid pointer to memory large enough to store ty
   * @param src pointer to the source. can be a pointer to initialized memory, or a pointer to an initialized constant.
   * @param env the environment of this compilation unit. 
   */
  void StoreAggregate(llvm::Type* ty, llvm::Value* dest, llvm::Value* src, const Environment& env);

}


/*------------------*/
