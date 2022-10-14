
#include "ast/Int.h"

#include "aux/Environment.h"

#include "visitor/AstVisitor.h"

namespace pink {
Int::Int(const Location &location, const long long value)
    : Ast(Ast::Kind::Int, location), value(value) {}

void Int::Accept(AstVisitor *visitor) const { visitor->Visit(this); }

auto Int::classof(const Ast *ast) -> bool {
  return ast->GetKind() == Ast::Kind::Int;
}

auto Int::ToString() const -> std::string { return std::to_string(value); }

/*
   ----------------------
    env |- [0-9]+ : Int
*/
auto Int::TypecheckV(const Environment &env) const -> Outcome<Type *, Error> {
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
