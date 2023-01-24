
#include "ast/Integer.h"

#include "aux/Environment.h"

namespace pink {

/*
   ----------------------
    env |- [0-9]+ : Int

auto Integer::TypecheckV(const Environment &env) const
    -> Outcome<Type *, Error> {
  return {env.types->GetIntType()};
}
*/

/*
   ----------------------
    env |- [0-9]+ : i64

auto Integer::Codegen(const Environment &env) const
    -> Outcome<llvm::Value *, Error> {
  return {env.instruction_builder->getInt64(value)};
}
*/
} // namespace pink
