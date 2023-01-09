#pragma once
#include "aux/Error.h"
#include "aux/Outcome.h"

#include "aux/Environment.h"

#include "ast/TypecheckVisitor.h"

namespace pink {
inline auto Typecheck(const Ast *ast, const Environment &env)
    -> Outcome<Type *, Error> {
  TypecheckVisitor visitor(env);
  return visitor.Compute(ast, visitor);
}
} // namespace pink