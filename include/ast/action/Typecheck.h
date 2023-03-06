#pragma once
#include "ast/Ast.h"

#include "type/TypeInterface.h"

#include "aux/Error.h"
#include "aux/Outcome.h"

namespace pink {
class Environment;
using TypecheckResult = Outcome<TypeInterface::Pointer, Error>;

/*
  #TODO: 2/18/2023 we are changing the conceptualization of what
  the Typechecker is. as a result, it is going to make more sense
  to define the TypecheckVisitor class in the header file, instead
  of only exporting the single procedure.

*/

/**
 * @brief Computes the [TypeInterface](#TypeInterface) of this [ast](#Ast)
 *
 * This function runs the, as far as I know, standard simple typing algorithm,
 * eagerly evaluated, with no implicit casting.
 *
 * \todo There are currently no syntactic forms
 * which allow for user types. (other than functions)
 *
 * @param ast the Ast to Typecheck
 * @param env The Environment to typecheck against, an
 * [Environment](#Environment) set up as if by
 * [CreateNativeEnvironment](#CreateNativeEnvironment) is suitable
 * @return Outcome<TypeInterface*, Error> if the type checking algorithm could
 * assign this term a [type](#TypeInterface) then the Outcome holds that type,
 * otherwise the Outcome holds the Error that the type checking algorithm
 * encountered.
 */
[[nodiscard]] auto Typecheck(const Ast::Pointer &ast, Environment &env) noexcept
    -> TypecheckResult;
} // namespace pink
