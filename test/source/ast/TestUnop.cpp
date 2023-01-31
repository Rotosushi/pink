#include "ast/TestUnop.h"
#include "Test.h"
#include "ast/Integer.h"
#include "ast/Unop.h"

#include "ast/action/ToString.h"
#include "ast/action/Typecheck.h"

#include "aux/Environment.h"

auto TestUnop(std::ostream &out) -> bool {
  bool        result = true;
  std::string name   = "pink::Unop";
  TestHeader(out, name);

  auto options = std::make_shared<pink::CLIOptions>();
  auto env     = pink::Environment::NewGlobalEnv(options);

  // "-1;"
  pink::InternedString minus = env->operators->Intern("-");
  // NOLINTBEGIN
  pink::Location       integer_loc(1, 2, 1, 3);
  pink::Location       unop_loc(1, 0, 1, 3);
  // NOLINTEND
  pink::Type::Pointer  integer_type = env->types->GetIntType();

  auto integer = std::make_unique<pink::Integer>(integer_loc, 1); // NOLINT
  pink::Ast *integer_pointer = integer.get();

  pink::Ast::Pointer unop =
      std::make_unique<pink::Unop>(unop_loc, minus, std::move(integer));

  result &=
      Test(out, "Unop::GetKind()", unop->GetKind() == pink::Ast::Kind::Unop);
  result &= Test(out, "Unop::classof()", llvm::isa<pink::Unop>(unop));
  result &= Test(out, "Unop::GetLoc()", unop->GetLocation() == unop_loc);

  std::string unop_str = "-1";
  result &= Test(out, "Unop::ToString()", ToString(unop) == unop_str);

  auto typecheck_result = Typecheck(unop, *env);
  result &=
      Test(out, "Unop::Typecheck()",
           typecheck_result && typecheck_result.GetFirst() == integer_type);

  return TestFooter(out, name, result);
}
