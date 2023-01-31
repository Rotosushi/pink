#include "ast/TestTuple.h"

#include "ast/Boolean.h"
#include "ast/Integer.h"
#include "ast/Tuple.h"

#include "ast/action/ToString.h"
#include "ast/action/Typecheck.h"

#include "Test.h"

#include "aux/Environment.h"

auto TestTuple(std::ostream &out) -> bool {
  bool        result = true;
  std::string name   = "pink::Tuple";
  TestHeader(out, name);

  auto options = std::make_shared<pink::CLIOptions>();
  auto env     = pink::Environment::NewGlobalEnv(options);

  // "(0, true, 1, false)"
  // NOLINTBEGIN
  pink::Location integer_zero_loc(1, 2, 1, 3);
  pink::Location boolean_true_loc(1, 5, 1, 9);
  pink::Location integer_one_loc(1, 11, 1, 12);
  pink::Location boolean_false_loc(1, 14, 1, 19);
  pink::Location tuple_loc(1, 1, 1, 20);
  auto integer_zero = std::make_unique<pink::Integer>(integer_zero_loc, 0);
  auto boolean_true = std::make_unique<pink::Boolean>(boolean_true_loc, true);
  auto integer_one  = std::make_unique<pink::Integer>(integer_one_loc, 1);
  pink::Ast::Pointer boolean_false =
      std::make_unique<pink::Boolean>(boolean_false_loc, false);
  // NOLINTEND

  std::vector<std::unique_ptr<pink::Ast>> tuple_elements;
  tuple_elements.emplace_back(std::move(integer_zero));
  tuple_elements.emplace_back(std::move(boolean_true));
  tuple_elements.emplace_back(std::move(integer_one));
  tuple_elements.emplace_back(std::move(boolean_false));

  pink::Ast::Pointer tuple =
      std::make_unique<pink::Tuple>(tuple_loc, std::move(tuple_elements));

  result &=
      Test(out, "Tuple::GetKind()", tuple->GetKind() == pink::Ast::Kind::Tuple);
  result &= Test(out, "Tuple::classof()", llvm::isa<pink::Tuple>(tuple));

  std::string tuple_str = "(0, true, 1, false)";
  result &= Test(out, "Tuple::ToString()", ToString(tuple) == tuple_str);

  auto *integer_type = env->types->GetIntType();
  auto *boolean_type = env->types->GetBoolType();
  auto *tuple_type   = env->types->GetTupleType(
      {integer_type, boolean_type, integer_type, boolean_type});
  auto typecheck_result = Typecheck(tuple, *env);
  result                &= Test(out, "Tuple::Typecheck()",
                                typecheck_result && typecheck_result.GetFirst() == tuple_type);

  return TestFooter(out, name, result);
}