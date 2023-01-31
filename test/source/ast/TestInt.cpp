#include "ast/TestInt.h"
#include "Test.h"

#include "ast/Integer.h"

#include "ast/action/ToString.h"
#include "ast/action/Typecheck.h"

#include "aux/Environment.h"

auto TestInt(std::ostream &out) -> bool {
  bool        result = true;
  std::string name   = "pink::Integer";
  TestHeader(out, name);

  auto options = std::make_shared<pink::CLIOptions>();
  auto env     = pink::Environment::NewGlobalEnv(options);

  // "42;"
  pink::Type::Pointer integer_type = env->types->GetIntType();
  pink::Location      integer_loc(1, 0, 1, 3);
  pink::Ast::Pointer  integer =
      std::make_unique<pink::Integer>(integer_loc, 42); // NOLINT

  result &= Test(out, "Int::GetKind()",
                 integer->GetKind() == pink::Ast::Kind::Integer);
  result &= Test(out, "Int::classof()", llvm::isa<pink::Integer>(integer));
  result &= Test(out, "Int::GetLoc()", integer->GetLocation() == integer_loc);

  result &= Test(out, "Int::ToString()", ToString(integer) == "42");

  auto typecheck_result = Typecheck(integer, *env);
  result &=
      Test(out, "Int::Typecheck()",
           typecheck_result && typecheck_result.GetFirst() == integer_type);

  return TestFooter(out, name, result);
}
