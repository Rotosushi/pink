#include "ast/Nil.h"

#include "aux/Environment.h"

namespace pink {

/*
    --------------------
      env |- nil : Nil

auto Nil::TypecheckV(const Environment &env) const -> Outcome<Type *, Error> {
  return {env.types->GetNilType()};
}
*/
/*
    ---------------------
    env |- nil : i1 (0)

auto Nil::Codegen(const Environment &env) const
    -> Outcome<llvm::Value *, Error> {
  return {env.instruction_builder->getFalse()};
}
*/
} // namespace pink
