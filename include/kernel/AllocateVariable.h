#pragma once

#include "aux/Environment.h"

#include "llvm/IR/Value.h"

namespace pink {
/**
 * @brief Allocate a variable. Performs global or local allocation
 *
 * @param name the name of the variable to allocate
 * @param type the type of the variable to allocate
 * @param env the environment of this compilation unit
 * @param initializer the variables initializer (can be nullptr)
 * @return llvm::Value* the pointer to the newly allocated variable
 */
auto AllocateVariable(const std::string &name, llvm::Type *type,
                      const Environment &env,
                      llvm::Value *initializer = nullptr) -> llvm::Value *;

/**
 * @brief Allocate a new global variable.
 *
 * @param name the name of the global
 * @param type the type of the global
 * @param env the environment of this compilation unit
 * @param initializer the global initializer. must be a llvm::Constant. (can be
 * nullptr)
 * @return llvm::GlobalVariable* the newly allocated global variable
 */
auto AllocateGlobal(const std::string &name, llvm::Type *type,
                    const Environment &env, llvm::Value *initializer = nullptr)
    -> llvm::GlobalVariable *;

/**
 * @brief Allocate a new local variable
 *
 * \note allocates at the beginning of the first basic block of the local
 * function
 *
 * @param name the name of the local
 * @param type the type of the local
 * @param env the environment of this compilation unit
 * @param initializer the local initializer. can be a constant or a value. (can
 * be nullptr)
 * @return llvm::AllocaInst* the newly allocated local variable
 */
auto AllocateLocal(const std::string &name, llvm::Type *type,
                   const Environment &env, llvm::Value *initializer = nullptr)
    -> llvm::AllocaInst *;

} // namespace pink