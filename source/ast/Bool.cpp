#include "ast/Bool.h"

#include "aux/Environment.h"

#include "visitor/AstVisitor.h"

namespace pink {
Bool::Bool(const Location &location, const bool value)
    : Ast(Ast::Kind::Bool, location), value(value) {}

void Bool::Accept(AstVisitor *visitor) const { visitor->Visit(this); }

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
  assert(GetType() != nullptr);
  return {env.instruction_builder->getInt1(value)};
}
} // namespace pink
