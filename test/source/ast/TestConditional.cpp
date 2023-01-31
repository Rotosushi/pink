#include "ast/TestConditional.h"
#include "Test.h"
#include "ast/Binop.h"
#include "ast/Block.h"
#include "ast/Boolean.h"
#include "ast/Conditional.h"
#include "ast/Integer.h"

#include "ast/action/ToString.h"
#include "ast/action/Typecheck.h"

#include "aux/Environment.h"

auto TestConditional(std::ostream &out) -> bool {
  bool        result = true;
  std::string name   = "pink::Conditional";
  TestHeader(out, name);

  auto options = std::make_shared<pink::CLIOptions>();
  auto env     = pink::Environment::NewGlobalEnv(options);

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
  auto integer_one  = std::make_unique<pink::Integer>(integer_one_loc, 1);
  auto integer_zero = std::make_unique<pink::Integer>(integer_zero_loc, 0);
  auto boolean_true = std::make_unique<pink::Boolean>(boolean_true_loc, true);
  auto boolean_false =
      std::make_unique<pink::Boolean>(boolean_false_loc, false);
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

  pink::Ast::Pointer conditional = std::make_unique<pink::Conditional>(
      conditional_loc, std::move(binop), std::move(then_block),
      std::move(else_block));

  result &= Test(out, "Conditional::GetKind()",
                 conditional->GetKind() == pink::Ast::Kind::Conditional);
  result &= Test(out, "Conditional::classof()",
                 llvm::isa<pink::Conditional>(conditional));

  result &= Test(out, "Conditional::GetLoc()",
                 conditional->GetLocation() == conditional_loc);

  std::string conditional_str = "if 1 == 0 then { true;\n } else { false;\n }";
  result                      &= Test(out, "Conditional::ToString()",
                                      ToString(conditional) == conditional_str);

  auto *boolean_type     = env->types->GetBoolType();
  auto  typecheck_result = Typecheck(conditional, *env);
  result &=
      Test(out, "Conditional::Typecheck()",
           typecheck_result && typecheck_result.GetFirst() == boolean_type);

  return TestFooter(out, name, result);
}