#include "ast/Nil.h"

#include "aux/Environment.h"

namespace pink {
Nil::Nil(const Location &location) : Ast(Ast::Kind::Nil, location) {}

auto Nil::classof(const Ast *ast) -> bool {
  return ast->getKind() == Ast::Kind::Nil;
}

auto Nil::ToString() const -> std::string { return {"nil"}; }

/*
    --------------------
      env |- nil : Nil
*/
auto Nil::GetypeV(const Environment &env) const -> Outcome<Type *, Error> {
  return {env.types->GetNilType()};
}

/*
    ---------------------
    env |- nil : i1 (0)
*/
auto Nil::Codegen(const Environment &env) const
    -> Outcome<llvm::Value *, Error> {
  return {env.instruction_builder->getFalse()};
}
} // namespace pink
