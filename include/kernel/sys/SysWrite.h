#pragma once

#include "aux/Environment.h"

#include "llvm/IR/Value.h"

namespace pink {
/**
 * @brief Calls sys_write passing in the data from a given slice
 *
 * \note must be a slice into an array of characters.
 *
 * @param file_descriptor the linux file descriptor to write to
 * @param slice_type the layout type of the slice
 * @param slice_ptr the pointer to the slice allocation
 * @param env the environment of this compilation unit
 * @return llvm::Value* the exit code of the call to sys_write
 */
auto CodegenWriteSlice(llvm::Value *file_descriptor,
                       llvm::StructType *slice_type, llvm::Value *slice_ptr,
                       const Environment &env) -> llvm::Value *;

/**
 * @brief Calls sys_write passing in the data from a given text allocation
 *
 * @param file_descriptor the linux file descriptor to write to
 * @param text_type the layout type of the text
 * @param text_ptr the pointer to the text allocation
 * @param env the environment of this compilation unit
 * @return llvm::Value* the exit code of the call to sys_write
 */
auto CodegenWriteText(llvm::Value *file_descriptor, llvm::StructType *text_type,
                      llvm::Value *text_ptr, const Environment &env)
    -> llvm::Value *;

/**
 * @brief Emits the inline assembly to call the sys_write x86-64 linux system
 * call
 *
 * @param file_descriptor the linux file descriptor to write to
 * @param size the size of the write
 * @param buffer the buffer holding the characters to write
 * @param env the environment of this compilation unit
 * @return llvm::Value* the exit code of the call to sys_write
 */
auto CodegenSysWrite(llvm::Value *file_descriptor, llvm::Value *size,
                     llvm::Value *buffer, const Environment &env)
    -> llvm::Value *;
} // namespace pink