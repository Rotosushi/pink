#include "ast/TestBind.h"
#include "Test.h"

#include "ast/Bind.h"
#include "ast/Boolean.h"

#include "ast/action/ToString.h"
#include "ast/action/Typecheck.h"

#include "aux/Environment.h"

auto TestBind(std::ostream &out) -> bool {
  bool        result = true;
  std::string name   = "pink::Bind";
  TestHeader(out, name);

  auto options = std::make_shared<pink::CLIOptions>();
  auto env     = pink::Environment::NewGlobalEnv(options);

  // "var v := true;\n"
  pink::InternedString variable = env->symbols->Intern("v");
  pink::Location       bind_loc(1, 0, 1, 15);     // NOLINT
  pink::Location       boolean_loc(1, 10, 1, 14); // NOLINT
  auto boolean = std::make_unique<pink::Boolean>(boolean_loc, true);

  pink::Ast::Pointer bind =
      std::make_unique<pink::Bind>(bind_loc, variable, std::move(boolean));

  result &=
      Test(out, "Bind::GetKind()", bind->GetKind() == pink::Ast::Kind::Bind);
  result &= Test(out, "Bind::classof()", llvm::isa<pink::Bind>(bind));

  result &= Test(out, "Bind::GetLoc()", bind_loc == bind->GetLocation());

  std::string bind_str = "v := true";

  result &= Test(out, "Bind::ToString()", ToString(bind) == bind_str);

  auto bind_type = Typecheck(bind, *env);

  result &=
      Test(out, "Bind::Typecheck()",
           bind_type && bind_type.GetFirst() == env->types->GetBoolType());

  return TestFooter(out, name, result);
}
