#include <sstream>
#include <string>

#include "Test.h"
#include "ast/Nil.h"
#include "ast/TestAstAndNil.h"

#include "aux/Environment.h"

auto TestAstAndNil(std::ostream &out) -> bool {
  bool result = true;
  out << "\n-----------------------\n";
  out << "Testing pink::Ast and pink::Nil: \n";

  auto options = std::make_shared<pink::CLIOptions>();
  auto env = pink::NewGlobalEnv(options);

  pink::Type *nil_t = env->types->GetNilType();
  pink::Location nil_loc(1, 14, 1, 15); // NOLINT
  std::unique_ptr<pink::Nil> nil = std::make_unique<pink::Nil>(nil_loc);

  result &= Test(out, "Nil::GetKind()", nil->GetKind() == pink::Ast::Kind::Nil);
  result &= Test(out, "Nil::classof()", nil->classof(nil.get()));

  result &= Test(out, "Nil::GetLoc()", nil_loc == nil->GetLoc());

  std::string nil_str = "nil";

  result &= Test(out, "Nil::ToString()", nil->ToString() == nil_str);

  pink::Outcome<pink::Type *, pink::Error> nil_type = nil->Typecheck(*env);

  result &=
      Test(out, "Nil::Typecheck()", nil_type && nil_type.GetFirst() == nil_t);

  result &= Test(out, "pink::Nil", result);
  out << "\n-----------------------\n";
  return result;
}
