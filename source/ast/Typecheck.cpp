#include "ast/Typecheck.h"
#include "ast/TypecheckVisitor.h"

namespace pink {

auto Typecheck(const Ast *ast, Environment &env) -> Outcome<Type *, Error> {
  TypecheckVisitor visitor(env);

  return visitor.Compute(ast, visitor);
}

} // namespace pink
