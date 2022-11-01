#include "ast/TestUnop.h"
#include "Test.h"
#include "ast/Int.h"
#include "ast/Unop.h"

#include "aux/Environment.h"

auto TestUnop(std::ostream &out) -> bool {
  bool result = true;

  out << "\n-----------------------\n";
  out << "Testing pink::Unop: \n";

  auto options = std::make_shared<pink::CLIOptions>();
  auto env = pink::Environment::NewGlobalEnv(options);

  // "-1;"
  pink::InternedString minus = env->operators->Intern("-");
  // NOLINTBEGIN
  pink::Location integer_loc(1, 2, 1, 3);
  pink::Location unop_loc(1, 0, 1, 3);
  // NOLINTEND
  pink::Type *integer_type = env->types->GetIntType();

  auto integer = std::make_unique<pink::Int>(integer_loc, 1); // NOLINT
  pink::Ast *integer_pointer = integer.get();

  auto unop = std::make_unique<pink::Unop>(unop_loc, minus, std::move(integer));

  result &=
      Test(out, "Unop::GetKind()", unop->GetKind() == pink::Ast::Kind::Unop);
  result &= Test(out, "Unop::classof()", unop->classof(unop.get()));
  result &= Test(out, "Unop::GetLoc()", unop->GetLoc() == unop_loc);
  result &= Test(out, "Unop::op", unop->GetOp() == minus);
  result &= Test(out, "Unop::term", unop->GetRight() == integer_pointer);

  std::string unop_str = std::string(minus) + integer_pointer->ToString();
  result &= Test(out, "Unop::ToString()", unop->ToString() == unop_str);

  auto typecheck_result = unop->Typecheck(*env);
  result &=
      Test(out, "Unop::Typecheck()",
           typecheck_result && typecheck_result.GetFirst() == integer_type);

  result &= Test(out, "pink::Unop", result);
  out << "\n-----------------------\n";
  return result;
}
