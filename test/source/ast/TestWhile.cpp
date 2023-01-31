#include "Test.h"

#include "ast/TestWhile.h"

#include "ast/Binop.h"
#include "ast/Block.h"
#include "ast/Boolean.h"
#include "ast/Integer.h"
#include "ast/While.h"

#include "ast/action/ToString.h"
#include "ast/action/Typecheck.h"

#include "aux/Environment.h"

auto TestWhile(std::ostream &out) -> bool {
  bool        result = true;
  std::string name   = "pink::While";
  TestHeader(out, name);

  auto options = std::make_shared<pink::CLIOptions>();
  auto env     = pink::Environment::NewGlobalEnv(options);

  // "while 1 == 0 do { true; }"
  //  ^     ^    ^    ^ ^     ^
  //  1     7    12  17 19    25
  // NOLINTBEGIN
  pink::Location integer_one_loc  = {1, 7, 1, 8};
  pink::Location integer_zero_loc = {1, 12, 1, 13};
  pink::Location binop_loc        = {1, 7, 1, 13};
  pink::Location boolean_true_loc = {1, 19, 1, 23};
  pink::Location block_loc        = {1, 17, 1, 25};
  pink::Location while_loc        = {1, 1, 1, 25};
  auto integer_zero = std::make_unique<pink::Integer>(integer_zero_loc, 0);
  auto integer_one  = std::make_unique<pink::Integer>(integer_one_loc, 1);
  // NOLINTEND

  const auto *cmpeq = env->operators->Intern("==");
  auto        binop = std::make_unique<pink::Binop>(
      binop_loc, cmpeq, std::move(integer_one), std::move(integer_zero));

  auto boolean_true = std::make_unique<pink::Boolean>(boolean_true_loc, true);

  std::vector<std::unique_ptr<pink::Ast>> block_statements;
  block_statements.emplace_back(std::move(boolean_true));
  auto block = std::make_unique<pink::Block>(
      block_loc, std::move(block_statements), env->bindings.get());

  pink::Ast::Pointer loop = std::make_unique<pink::While>(
      while_loc, std::move(binop), std::move(block));

  result &=
      Test(out, "While::GetKind()", loop->GetKind() == pink::Ast::Kind::While);
  result &= Test(out, "While::classof()", llvm::isa<pink::While>(loop));
  result &= Test(out, "While::GetLoc()", loop->GetLocation() == while_loc);

  std::string while_str = "while 1 == 0 do { true;\n }";
  result &= Test(out, "While::ToString()", ToString(loop) == while_str);

  auto *nil_type         = env->types->GetNilType();
  auto  typecheck_result = Typecheck(loop, *env);
  result                 &= Test(out, "While::Typecheck()",
                                 typecheck_result && typecheck_result.GetFirst() == nil_type);

  return TestFooter(out, name, result);
}