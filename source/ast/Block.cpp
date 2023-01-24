
#include "ast/Block.h"
#include "aux/Environment.h"

namespace pink {
/*
  The type of a block is the type of it's last statement.

  env |- {s0;s1;...;sn;}, s0 : T0, s1 : T1, ..., sn : Tn
     ----------------------------------------------
            env |- {s0;s1;...;sn;} : Tn


auto Block::TypecheckV(const Environment &env) const -> Outcome<Type *, Error> {
  Outcome<Type *, Error> result = Error();

  for (const auto &stmt : statements) {
    result = stmt->Typecheck(env);

    if (!result) {
      return result;
    }
  }

  return result;
}
*/

/*
  The value of a block is the value of it's last statement.

auto Block::Codegen(const Environment &env) const
    -> Outcome<llvm::Value *, Error> {
  assert(GetType() != nullptr);
  Outcome<llvm::Value *, Error> result = Error();

  for (const auto &stmt : statements) {
    result = stmt->Codegen(env);

    if (!result) {
      return result;
    }
  }

  return result;
}
*/
} // namespace pink
