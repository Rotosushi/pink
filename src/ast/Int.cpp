
#include "ast/Int.h"

#include "aux/Environment.h"

namespace pink {
Int::Int(const Location &location, const long long value)
    : Ast(Ast::Kind::Int, location), value(value) {}

auto Int::classof(const Ast *a) -> bool {
  return a->getKind() == Ast::Kind::Int;
}

auto Int::ToString() const -> std::string { return std::to_string(value); }

/*
   ----------------------
    env |- [0-9]+ : Int
*/
auto Int::GetypeV(const Environment &env) const -> Outcome<Type *, Error> {
  return {env.types->GetIntType()};
}

/*
   ----------------------
    env |- [0-9]+ : i64 (value)
*/
auto Int::Codegen(const Environment &env) const
    -> Outcome<llvm::Value *, Error> {
  return {env.instruction_builder->getInt64(value)};
}
} // namespace pink
