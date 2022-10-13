#include "ast/Bool.h"

#include "aux/Environment.h"

namespace pink {
Bool::Bool(const Location &location, const bool value)
    : Ast(Ast::Kind::Bool, location), value(value) {}

auto Bool::classof(const Ast *ast) -> bool {
  return ast->GetKind() == Ast::Kind::Bool;
}

auto Bool::ToString() const -> std::string {
  if (value) {
    return "true";
  }

  return "false";
}

/*
   ---------------------
    env |- true : Bool

   ---------------------
    env |- false : Bool
*/
auto Bool::TypecheckV(const Environment &env) const -> Outcome<Type *, Error> {
  return {env.types->GetBoolType()};
}

/*
    ---------------------
    env |- true : i1 (1)

   ---------------------
    env |- false : i1 (0)
*/
auto Bool::Codegen(const Environment &env) const
    -> Outcome<llvm::Value *, Error> {
  return {env.instruction_builder->getInt1(value)};
}
} // namespace pink
