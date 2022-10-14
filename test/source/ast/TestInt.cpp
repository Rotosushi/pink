#include "ast/TestInt.h"
#include "Test.h"

#include "ast/Int.h"

#include "aux/Environment.h"

auto TestInt(std::ostream &out) -> bool {
  bool result = true;

  out << "\n-----------------------\n";
  out << "Testing pink::Int: \n";

  auto options = std::make_shared<pink::CLIOptions>();
  auto env = pink::NewGlobalEnv(options);

  // "42;"
  pink::Type *integer_type = env->types->GetIntType();
  pink::Location integer_loc(1, 0, 1, 3);
  auto integer = std::make_unique<pink::Int>(integer_loc, 42); // NOLINT

  result &=
      Test(out, "Int::GetKind()", integer->GetKind() == pink::Ast::Kind::Int);
  result &= Test(out, "Int::classof()", integer->classof(integer.get()));
  result &= Test(out, "Int::GetLoc()", integer->GetLoc() == integer_loc);
  result &= Test(out, "Int::value", integer->GetValue() == 42); // NOLINT
  result &= Test(out, "Int::ToString()", integer->ToString() == "42");

  auto typecheck_result = integer->Typecheck(*env);
  result &=
      Test(out, "Int::Typecheck()",
           typecheck_result && typecheck_result.GetFirst() == integer_type);

  result &= Test(out, "pink::Int", result);
  out << "\n-----------------------\n";
  return result;
}
