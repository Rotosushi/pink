#include <sstream>
#include <string>

#include "Test.h"
#include "ast/Nil.h"
#include "ast/TestNil.h"

#include "ast/action/ToString.h"
#include "ast/action/Typecheck.h"

#include "aux/Environment.h"

auto TestNil(std::ostream &out) -> bool {
  bool        result = true;
  std::string name   = "pink::Nil";
  TestHeader(out, name);

  auto options = std::make_shared<pink::CLIOptions>();
  auto env     = pink::Environment::NewGlobalEnv(options);

  pink::Type::Pointer nil_t = env->types->GetNilType();
  pink::Location      nil_loc(1, 14, 1, 15); // NOLINT
  pink::Ast::Pointer  nil = std::make_unique<pink::Nil>(nil_loc);

  result &= Test(out, "Nil::GetKind()", nil->GetKind() == pink::Ast::Kind::Nil);
  result &= Test(out, "Nil::classof()", llvm::isa<pink::Nil>(nil));

  result &= Test(out, "Nil::GetLoc()", nil_loc == nil->GetLocation());

  std::string nil_str = "nil";

  result &= Test(out, "Nil::ToString()", ToString(nil) == nil_str);

  auto nil_result = Typecheck(nil, *env);

  result &= Test(out, "Nil::Typecheck()",
                 nil_result && nil_result.GetFirst() == nil_t);

  return TestFooter(out, name, result);
}
