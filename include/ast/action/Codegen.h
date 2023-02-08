#pragma once
#include "ast/Ast.h"

#include "aux/Error.h"
#include "aux/Outcome.h"

#include "llvm/IR/Value.h"

namespace pink {
class Environment;
using CodegenResult = llvm::Value *;
/**
 * @brief Computes the LLVM Intermediate Representation ([IR]) corresponding
 * to this Ast.
 *
 * [IR]: https://llvm.org/docs/LangRef.html "IR"
 *
 * Internally this function uses the [instruction_builder] to construct the
 * [IR]. and does so with respect to the [symbol_table], [binop_table],
 * [unop_table], and [module], held within the environment parameter.
 * (this is not intended as a complete list)
 *
 * [symbol_table]: @ref SymbolTable "symbol table"
 * [binop_table]: @ref BinopTable "binop table"
 * [unop_table]: @ref UnopTable "unop table"
 * [instruction_builder]: https://llvm.org/doxygen/classllvm_1_1IRBuilder.html
 * "instruction builder" [module]:
 * https://llvm.org/doxygen/classllvm_1_1Module.html "module"
 *
 * @param env The Environment to generate code against, an
 * [Environment](#Environment) set up as if by
 * [CreateNativeGlobalEnv](#CreateNativeGlobalEnv) is suitable
 * @return CodegenResult if the code generator could assign
 * this term a [value] then the Outcome holds this value, otherwise the
 * Outcome holds the [Error] the code generator constructed.
 */
[[nodiscard]] auto Codegen(const Ast::Pointer &ast, Environment &env) noexcept
    -> CodegenResult;
} // namespace pink
