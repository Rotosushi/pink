#include "ast/Boolean.h"

#include "aux/Environment.h"

#include "visitor/AstVisitor.h"

namespace pink {
Boolean::Boolean(const Location &location, const bool value)
    : Ast(Ast::Kind::Boolean, location), value(value) {}

void Boolean::Accept(const ConstAstVisitor *visitor) const {
  visitor->Visit(this);
}

auto Boolean::classof(const Ast *ast) -> bool {
  return ast->GetKind() == Ast::Kind::Boolean;
}

auto Boolean::ToString() const -> std::string {
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
auto Boolean::TypecheckV(const Environment &env) const
    -> Outcome<Type *, Error> {
  return {env.types->GetBoolType()};
}

/*
    ---------------------
    env |- true : i1 (1)

   ---------------------
    env |- false : i1 (0)
*/
auto Boolean::Codegen(const Environment &env) const
    -> Outcome<llvm::Value *, Error> {
  assert(GetType() != nullptr);
  return {env.instruction_builder->getInt1(value)};
}
} // namespace pink
