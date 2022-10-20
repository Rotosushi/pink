#include "Test.h"

#include "ast/TestWhile.h"

#include "ast/Binop.h"
#include "ast/Block.h"
#include "ast/Bool.h"
#include "ast/Int.h"
#include "ast/While.h"

#include "aux/Environment.h"

auto TestWhile(std::ostream &out) -> bool {
  bool result = true;
  out << "\n-----------------------------\n";
  out << "Testing pink::While: \n";

  auto options = std::make_shared<pink::CLIOptions>();
  auto env = pink::NewGlobalEnv(options);

  // "while 1 == 0 do { true; }"
  //  ^     ^    ^    ^ ^     ^
  //  1     7    12  17 19    25
  // NOLINTBEGIN
  pink::Location integer_one_loc = {1, 7, 1, 8};
  pink::Location integer_zero_loc = {1, 12, 1, 13};
  pink::Location binop_loc = {1, 7, 1, 13};
  pink::Location boolean_true_loc = {1, 19, 1, 23};
  pink::Location block_loc = {1, 17, 1, 25};
  pink::Location while_loc = {1, 1, 1, 25};
  auto integer_zero = std::make_unique<pink::Int>(integer_zero_loc, 0);
  auto integer_one = std::make_unique<pink::Int>(integer_one_loc, 1);
  // NOLINTEND

  const auto *cmpeq = env->operators->Intern("==");
  auto binop = std::make_unique<pink::Binop>(
      binop_loc, cmpeq, std::move(integer_one), std::move(integer_zero));

  auto boolean_true = std::make_unique<pink::Bool>(boolean_true_loc, true);

  std::vector<std::unique_ptr<pink::Ast>> block_statements;
  block_statements.emplace_back(std::move(boolean_true));
  auto block = std::make_unique<pink::Block>(block_loc, block_statements);

  auto loop = std::make_unique<pink::While>(while_loc, std::move(binop),
                                            std::move(block));

  result &=
      Test(out, "While::GetKind()", loop->GetKind() == pink::Ast::Kind::While);
  result &= Test(out, "While::classof()", loop->classof(loop.get()));
  result &= Test(out, "While::GetLoc()", loop->GetLoc() == while_loc);

  std::string while_str = "while 1 == 0 do { true;\n }";
  result &= Test(out, "While::ToString()", loop->ToString() == while_str);

  auto *void_type = env->types->GetVoidType();
  auto typecheck_result = loop->Typecheck(*env);
  result &= Test(out, "While::Typecheck()",
                 typecheck_result && typecheck_result.GetFirst() == void_type);

  return Test(out, "pink::While", result);
}