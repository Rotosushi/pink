
#include "ast/Block.h"
#include "aux/Environment.h"

#include "visitor/AstVisitor.h"

namespace pink {
Block::Block(const Location &location) : Ast(Ast::Kind::Block, location) {}

Block::Block(const Location &location,
             std::vector<std::unique_ptr<Ast>> &stmnts)
    : Ast(Ast::Kind::Block, location), statements(std::move(stmnts)) {}

void Block::Accept(AstVisitor *visitor) const { visitor->Visit(this); }

auto Block::begin() const -> Block::const_iterator {
  return statements.begin();
}

auto Block::end() const -> Block::const_iterator { return statements.end(); }

auto Block::classof(const Ast *ast) -> bool {
  return ast->GetKind() == Ast::Kind::Block;
}

/// \todo Make Block::ToString prepend the
/// correct number of tabs to properly
/// indent the block at the current level
/// of nesting.
auto Block::ToString() const -> std::string {
  std::string result = "{ ";

  auto convert = [&result](const std::unique_ptr<Ast> &statement) {
    result += statement->ToString() + ";\n";
  };

  std::for_each(statements.begin(), statements.end(), convert);

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