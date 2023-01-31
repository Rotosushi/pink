#include "ast/TestBinop.h"
#include "Test.h"

#include "ast/Binop.h"
#include "ast/Integer.h"

#include "ast/action/ToString.h"
#include "ast/action/Typecheck.h"

#include "aux/Environment.h"

auto TestBinop(std::ostream &out) -> bool {
  bool        result = true;
  std::string name   = "pink::Binop";
  TestHeader(out, name);

  auto options = std::make_shared<pink::CLIOptions>();
  auto env     = pink::Environment::NewGlobalEnv(options);

  // "1 + 1"
  pink::InternedString plus = env->operators->Intern("+");
  pink::Location       integer_1_loc(1, 0, 1, 1); // NOLINT
  pink::Location       integer_2_loc(1, 4, 1, 5); // NOLINT
  pink::Location       binop_loc(1, 0, 1, 5);     // NOLINT
  pink::Type::Pointer  integer_type = env->types->GetIntType();
  auto integer_1           = std::make_unique<pink::Integer>(integer_1_loc, 1);
  auto integer_2           = std::make_unique<pink::Integer>(integer_2_loc, 1);
  pink::Ast::Pointer binop = std::make_unique<pink::Binop>(
      binop_loc, plus, std::move(integer_1), std::move(integer_2));

  result &=
      Test(out, "Binop::GetKind()", binop->GetKind() == pink::Ast::Kind::Binop);
  result &= Test(out, "Binop::classof()", llvm::isa<pink::Binop>(binop));
  result &= Test(out, "Binop::GetLoc()", binop_loc == binop->GetLocation());

  std::string binop_str = "1 + 1";

  result &= Test(out, "Binop::ToString()", ToString(binop) == binop_str);

  auto binop_type = Typecheck(binop, *env);
  result          &= Test(out, "Binop::Typecheck()",
                          binop_type && binop_type.GetFirst() == integer_type);

  return TestFooter(out, name, result);
}
