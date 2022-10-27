#include "ast/Variable.h"
#include "aux/Environment.h"

#include "visitor/AstVisitor.h"

#include "kernel/LoadValue.h"

namespace pink {
Variable::Variable(const Location &location, InternedString symbol)
    : Ast(Ast::Kind::Variable, location), symbol(symbol) {}

void Variable::Accept(AstVisitor *visitor) const { visitor->Visit(this); }

auto Variable::classof(const Ast *ast) -> bool {
  return ast->GetKind() == Ast::Kind::Variable;
}

auto Variable::ToString() const -> std::string { return {symbol}; }

/*
        env |- x is-in env, x : T
  -----------------------------
                  env |- x : T
*/
auto Variable::TypecheckV(const Environment &env) const
    -> Outcome<Type *, Error> {
  auto bound = env.bindings->Lookup(symbol);

  if (bound.has_value()) {
    return {bound->first};
  }

  std::string errmsg = std::string("unknown symbol: ") + symbol;
  return {Error(Error::Code::NameNotBoundInScope, GetLoc(), errmsg)};
}

auto Variable::Codegen(const Environment &env) const
    -> Outcome<llvm::Value *, Error> {
  auto bound = env.bindings->Lookup(symbol);

  if (bound.has_value()) {
    assert(bound->second != nullptr);
    auto *bound_type = bound->first->ToLLVM(env);

    return LoadValue(bound_type, bound->second, env);
  }

  std::string errmsg = std::string("unknown symbol: ") + symbol;
  return {Error(Error::Code::NameNotBoundInScope, GetLoc(), errmsg)};
}
} // namespace pink
