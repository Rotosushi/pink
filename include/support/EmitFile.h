/**
 * @file EmitFile.h
 * @brief Header for Emitting files
 * @version 0.1
 */
#pragma once

#include "aux/Environment.h"

namespace pink {
/**
 * @brief Emit an object file from the llvm_module in env, named filename
 *
 * @param env the env holding the llvm_module to emit
 * @param filename the name of the object file to emit
 */
void EmitObjectFile(const Environment &env, const std::string &filename);

/**
 * @brief Emit an assembly file from the llvm_module in env, named filename
 *
 * @param env the env holding the llvm_module to emit
 * @param filename the name of the object file to emit
 */
void EmitAssemblyFile(const Environment &env, const std::string &filename);

/**
 * @brief Emit an llvm source file from the llvm_module in env, named filename
 *
 * @param env the env holding the llvm_module to emit
 * @param filename the name of the llvm source file to emit
 */
void EmitLLVMFile(const Environment &env, const std::string &filename);
} // namespace pink