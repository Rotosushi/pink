#include "ast/TestBool.h"
#include "Test.h"

#include "ast/Boolean.h"

#include "ast/action/ToString.h"
#include "ast/action/Typecheck.h"

#include "aux/Environment.h"

auto TestBool(std::ostream &out) -> bool {
  bool        result = true;
  std::string name   = "pink::Boolean";
  TestHeader(out, name);

  auto options = std::make_shared<pink::CLIOptions>();
  auto env     = pink::Environment::NewGlobalEnv(options);

  // "true;"
  pink::Location     boolean_loc(1, 0, 1, 4); // NOLINT
  pink::Ast::Pointer boolean =
      std::make_unique<pink::Boolean>(boolean_loc, true);
  pink::Type::Pointer boolean_type = env->types->GetBoolType();

  result &= Test(out, "Bool::GetKind()",
                 boolean->GetKind() == pink::Ast::Kind::Boolean);
  result &= Test(out, "Bool::classof()", llvm::isa<pink::Boolean>(boolean));
  result &= Test(out, "Bool::GetLoc()", boolean->GetLocation() == boolean_loc);
  result &= Test(out, "Bool::ToString()", ToString(boolean) == "true");

  auto typecheck_result = Typecheck(boolean, *env);
  result &=
      Test(out, "Bool::Typecheck()",
           typecheck_result && typecheck_result.GetFirst() == boolean_type);

  return TestFooter(out, name, result);
}
