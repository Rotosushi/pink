#include "ast/TestConditional.h"
#include "Test.h"
#include "ast/Binop.h"
#include "ast/Block.h"
#include "ast/Bool.h"
#include "ast/Conditional.h"
#include "ast/Int.h"

#include "aux/Environment.h"

auto TestConditional(std::ostream &out) -> bool {
  bool result = true;
  out << "\n---------------------------------\n";
  out << "Testing pink::Array:\n";

  auto options = std::make_shared<pink::CLIOptions>();
  auto env = pink::Environment::NewGlobalEnv(options);

  // 1: "if 1 == 0 then { true; } else { false; }"
  //     ^  ^    ^      ^ ^     ^      ^ ^      ^
  //     1  4    9     16 18    24    31 33     40
  //  NOLINTBEGIN
  pink::Location integer_one_loc(1, 4, 1, 5);
  pink::Location integer_zero_loc(1, 9, 1, 10);
  pink::Location binop_loc(1, 4, 1, 10);
  pink::Location boolean_true_loc(1, 18, 1, 22);
  pink::Location then_block_loc(1, 16, 1, 24);
  pink::Location boolean_false_loc(1, 33, 1, 38);
  pink::Location else_block_loc(1, 31, 1, 40);
  pink::Location conditional_loc(1, 1, 1, 40);
  auto integer_one = std::make_unique<pink::Int>(integer_one_loc, 1);
  auto integer_zero = std::make_unique<pink::Int>(integer_zero_loc, 0);
  auto boolean_true = std::make_unique<pink::Bool>(boolean_true_loc, true);
  auto boolean_false = std::make_unique<pink::Bool>(boolean_false_loc, false);
  // NOLINTEND
  std::vector<std::unique_ptr<pink::Ast>> then_statements;
  then_statements.emplace_back(std::move(boolean_true));

  std::vector<std::unique_ptr<pink::Ast>> else_statements;
  else_statements.emplace_back(std::move(boolean_false));

  auto then_block =
      std::make_unique<pink::Block>(then_block_loc, then_statements);
  auto else_block =
      std::make_unique<pink::Block>(else_block_loc, else_statements);
  pink::InternedString cmpeq = env->operators->Intern("==");

  auto binop = std::make_unique<pink::Binop>(
      binop_loc, cmpeq, std::move(integer_one), std::move(integer_zero));

  auto conditional = std::make_unique<pink::Conditional>(
      conditional_loc, std::move(binop), std::move(then_block),
      std::move(else_block));

  result &= Test(out, "Conditional::GetKind()",
                 conditional->GetKind() == pink::Ast::Kind::Conditional);
  result &= Test(out, "Conditional::classof()",
                 conditional->classof(conditional.get()));

  result &= Test(out, "Conditional::GetLoc()",
                 conditional->GetLoc() == conditional_loc);

  std::string conditional_str = "if 1 == 0 then { true;\n } else { false;\n }";
  result &= Test(out, "Conditional::ToString()",
                 conditional->ToString() == conditional_str);

  auto *boolean_type = env->types->GetBoolType();
  auto typecheck_result = conditional->Typecheck(*env);
  result &=
      Test(out, "Conditional::Typecheck()",
           typecheck_result && typecheck_result.GetFirst() == boolean_type);

  Test(out, "pink::Conditional", result);
  return result;
}