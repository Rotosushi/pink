#include "ast/Boolean.h"

#include "aux/Environment.h"

namespace pink {
/*
   ---------------------
    env |- true : Bool

   ---------------------
    env |- false : Bool

auto Boolean::TypecheckV(const Environment &env) const
    -> Outcome<Type *, Error> {
  return {env.types->GetBoolType()};
}
*/

/*
    ---------------------
    env |- true : i1 (1)

   ---------------------
    env |- false : i1 (0)

auto Boolean::Codegen(const Environment &env) const
    -> Outcome<llvm::Value *, Error> {
  assert(GetType() != nullptr);
  return {env.instruction_builder->getInt1(value)};
}
*/
} // namespace pink
