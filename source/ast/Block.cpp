
#include "ast/Block.h"
#include "aux/Environment.h"

namespace pink {
Block::Block(const Location &location) : Ast(Ast::Kind::Block, location) {}

Block::Block(const Location &location,
             std::vector<std::unique_ptr<Ast>> &stmnts)
    : Ast(Ast::Kind::Block, location), statements(std::move(stmnts)) {}

auto Block::classof(const Ast *ast) -> bool {
  return ast->GetKind() == Ast::Kind::Block;
}

/// \todo Make Block::ToString prepend the
/// correct number of tabs to properly
/// indent the block at the current level
/// of nesting.
auto Block::ToString() const -> std::string {
  std::string result = "{ ";

  for (const auto &statement : statements) {
    result += statement->ToString() + ";\n";
  }

  result += " }";

  return result;
}

/*
  The type of a block is the type of it's last statement.

  env |- {s0;s1;...;sn;}, s0 : T0, s1 : T1, ..., sn : Tn
     ----------------------------------------------
            env |- {s0;s1;...;sn;} : Tn

*/
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

/*
  The value of a block is the value of it's last statement.
*/
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
} // namespace pink
