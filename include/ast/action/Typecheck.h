#pragma once
#include "ast/Ast.h"

#include "type/Type.h"

#include "aux/Error.h"
#include "aux/Outcome.h"

namespace pink {
class Environment;
using TypecheckResult = Outcome<Type::Pointer, Error>;
/**
 * @brief Computes the [Type](#Type) of this [ast](#Ast)
 *
 * This function runs the, as far as I know, standard simple typing algorithm,
 * eagerly evaluated, with no implicit casting. There are currently
 * no syntactic forms which allow for user types. (other than functions)
 *
 * \todo implement some form of user defined types
 *
 * @param ast the Ast to Typecheck
 * @param env The Environment to typecheck against, an
 * [Environment](#Environment) set up as if by [NewGlobalEnv](#NewGlobalEnv)
 * is suitable
 * @return Outcome<Type*, Error> if the type checking algorithm could assign
 * this term a [type](#Type) then the Outcome holds that type, otherwise the
 * Outcome holds the Error that the type checking algorithm encountered.
 */
[[nodiscard]] auto Typecheck(const Ast::Pointer &ast, Environment &env) noexcept
    -> TypecheckResult;

[[nodiscard]] auto Typecheck(const Ast *ast, Environment &env) noexcept
    -> TypecheckResult;
} // namespace pink
