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

namespace pink {
class CompilationUnit;
using TypecheckResult = Outcome<Type::Pointer, Error>;

/**
 * @brief Computes the [Type](#Type) of this [ast](#Ast)
 *
 * This function runs the, as far as I know, standard simple typing algorithm,
 * eagerly evaluated, with no implicit casting.
 *
 * @param ast the Ast to Typecheck
 * @param env The CompilationUnit to typecheck against, an
 * [CompilationUnit](#CompilationUnit) set up as if by
 * [CreateNativeCompilationUnit](#CreateNativeCompilationUnit) is suitable
 * @return Outcome<Type*, Error> if the type checking algorithm could
 * assign this term a [type](#Type) then the Outcome holds that type,
 * otherwise the Outcome holds the Error that the type checking algorithm
 * encountered.
 */
[[nodiscard]] auto Typecheck(const Ast::Pointer &ast,
                             CompilationUnit &env) noexcept -> TypecheckResult;
} // namespace pink
