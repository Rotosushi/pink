// Copyright (C) 2023 cadence
//
// This file is part of pink.
//
// pink is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// pink is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with pink.  If not, see <http://www.gnu.org/licenses/>.

#pragma once
#include "ast/Ast.h"

#include "aux/Error.h"
#include "aux/Outcome.h"

#include "llvm/IR/Value.h"

namespace pink {
class CompilationUnit;
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
 * @param env The CompilationUnit to generate code against, an
 * [CompilationUnit](#CompilationUnit) set up as if by
 * [CreateNativeEnvironment](#CreateNativeEnvironment) is suitable
 * @return CodegenResult if the code generator could assign
 * this term a [value] then the Outcome holds this value, otherwise the
 * Outcome holds the [Error] the code generator constructed.
 */
[[nodiscard]] auto Codegen(const Ast::Pointer &ast,
                           CompilationUnit    &env) noexcept -> CodegenResult;
} // namespace pink
