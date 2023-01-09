
#include "ast/Integer.h"

#include "aux/Environment.h"

#include "visitor/AstVisitor.h"

namespace pink {
Integer::Integer(const Location &location, const long long value)
    : Ast(Ast::Kind::Integer, location), value(value) {}

void Integer::Accept(const ConstAstVisitor *visitor) const {
  visitor->Visit(this);
}

auto Integer::classof(const Ast *ast) -> bool {
  return ast->GetKind() == Ast::Kind::Integer;
}

auto Integer::ToString() const -> std::string { return std::to_string(value); }

/*
   ----------------------
    env |- [0-9]+ : Int
*/
auto Integer::TypecheckV(const Environment &env) const
    -> Outcome<Type *, Error> {
  return {env.types->GetIntType()};
}

/*
   ----------------------
    env |- [0-9]+ : i64
*/
auto Integer::Codegen(const Environment &env) const
    -> Outcome<llvm::Value *, Error> {
  return {env.instruction_builder->getInt64(value)};
}
} // namespace pink
