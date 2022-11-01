
#include "ast/TestBlock.h"
#include "Test.h"

#include "ast/Bind.h"
#include "ast/Binop.h"
#include "ast/Block.h"
#include "ast/Bool.h"
#include "ast/Int.h"

#include "aux/Environment.h"

auto TestBlock(std::ostream &out) -> bool {
  bool result = true;

  out << "\n-----------------------\n";
  out << "Testing pink::Block: \n";

  auto options = std::make_shared<pink::CLIOptions>();
  auto env = pink::Environment::NewGlobalEnv(options);

  // "var x := true; 3 + 5;"
  pink::InternedString plus = env->operators->Intern("+");
  pink::InternedString variable = env->symbols->Intern("x");
  // NOLINTBEGIN
  pink::Location boolean_loc(1, 10, 1, 14);
  pink::Location bind_loc(1, 0, 1, 14);
  pink::Location integer_1_loc(1, 16, 1, 17);
  pink::Location integer_2_loc(1, 20, 1, 21);
  pink::Location binop_loc(1, 16, 1, 21);
  pink::Location block_loc(1, 0, 1, 21);
  // NOLINTEND
  pink::Type *integer_type = env->types->GetIntType();

  auto boolean = std::make_unique<pink::Bool>(boolean_loc, true);

  auto bind =
      std::make_unique<pink::Bind>(bind_loc, variable, std::move(boolean));

  auto integer_1 = std::make_unique<pink::Int>(integer_1_loc, 3); // NOLINT

  auto integer_2 = std::make_unique<pink::Int>(integer_2_loc, 5); // NOLINT

  auto binop = std::make_unique<pink::Binop>(
      binop_loc, plus, std::move(integer_1), std::move(integer_2));

  std::vector<std::unique_ptr<pink::Ast>> stmts;
  stmts.emplace_back(std::move(bind));
  stmts.emplace_back(std::move(binop));

  auto block = std::make_unique<pink::Block>(block_loc, stmts);

  result &=
      Test(out, "Block::getKind()", block->GetKind() == pink::Ast::Kind::Block);
  result &= Test(out, "Block::GetLoc()", block->GetLoc() == block_loc);
  result &= Test(out, "Block::classof()", block->classof(block.get()));

  auto iter = block->begin();
  result &= Test(out, "Block::iterator", iter != block->end());

  auto typecheck_result = block->Typecheck(*env);

  result &=
      Test(out, "Block::Typecheck()",
           (typecheck_result) && (typecheck_result.GetFirst() == integer_type));

  result &= Test(out, "pink::Block", result);
  out << "\n-----------------------\n";
  return result;
}
