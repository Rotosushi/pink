#pragma once
#include "aux/Error.h"
#include "aux/Outcome.h"

#include "aux/Environment.h"

namespace pink {
auto Typecheck(const Ast *ast, const Environment &env)
    -> Outcome<Type *, Error>;
} // namespace pink