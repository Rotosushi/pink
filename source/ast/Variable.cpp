#include "ast/Variable.h"
#include "aux/Environment.h"

namespace pink {
Variable::Variable(const Location &location, InternedString symbol)
    : Ast(Ast::Kind::Variable, location), symbol(symbol) {}

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

    auto type_result = bound->first->Codegen(env);

    if (!type_result) {
      return {type_result.GetSecond()};
    }

    auto *bound_type = type_result.GetFirst();

    if (llvm::isa<llvm::FunctionType>(bound_type) ||
        (llvm::isa<llvm::ArrayType>(bound_type)) ||
        (llvm::isa<llvm::StructType>(bound_type)) ||
        env.flags->OnTheLHSOfAssignment() || env.flags->WithinAddressOf()) {
      return {bound->second};
    }

    return {
        env.instruction_builder->CreateLoad(bound_type, bound->second, symbol)};
  }

  std::string errmsg = std::string("unknown symbol: ") + symbol;
  return {Error(Error::Code::NameNotBoundInScope, GetLoc(), errmsg)};
}
} // namespace pink
