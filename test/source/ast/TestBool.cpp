#include "ast/TestBool.h"
#include "Test.h"

#include "ast/Bool.h"

#include "aux/Environment.h"

auto TestBool(std::ostream &out) -> bool {
  bool result = true;

  out << "\n-----------------------\n";
  out << "Testing pink::Bool: \n";

  auto options = std::make_shared<pink::CLIOptions>();
  auto env = pink::NewGlobalEnv(options);

  // "true;"
  pink::Location boolean_loc(1, 0, 1, 4); // NOLINT
  std::unique_ptr<pink::Bool> boolean =
      std::make_unique<pink::Bool>(boolean_loc, true);
  pink::Type *boolean_type = env->types->GetBoolType();

  result &=
      Test(out, "Bool::GetKind()", boolean->GetKind() == pink::Ast::Kind::Bool);
  result &= Test(out, "Bool::classof()", boolean->classof(boolean.get()));
  result &= Test(out, "Bool::GetLoc()", boolean->GetLoc() == boolean_loc);
  result &= Test(out, "Bool::value", boolean->GetValue());
  result &= Test(out, "Bool::ToString()", boolean->ToString() == "true");

  auto typecheck_result = boolean->Typecheck(*env);
  result &=
      Test(out, "Bool::Typecheck()",
           typecheck_result && typecheck_result.GetFirst() == boolean_type);

  result &= Test(out, "pink::Result", result);
  out << "\n-----------------------\n";
  return result;
}
