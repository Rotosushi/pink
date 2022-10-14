#include "ast/TestBind.h"
#include "Test.h"

#include "ast/Bind.h"
#include "ast/Bool.h"

#include "aux/Environment.h"

auto TestBind(std::ostream &out) -> bool {
  bool result = true;
  out << "\n-----------------------\n";
  out << "Testing pink::Bind: \n";

  auto options = std::make_shared<pink::CLIOptions>();
  auto env = pink::NewGlobalEnv(options);

  // "var v := true;\n"
  pink::InternedString variable = env->symbols->Intern("v");
  pink::Location bind_loc(1, 0, 1, 15);     // NOLINT
  pink::Location boolean_loc(1, 10, 1, 14); // NOLINT
  auto boolean = std::make_unique<pink::Bool>(boolean_loc, true);
  pink::Ast *boolean_pointer = boolean.get();

  auto bind =
      std::make_unique<pink::Bind>(bind_loc, variable, std::move(boolean));

  result &=
      Test(out, "Bind::GetKind()", bind->GetKind() == pink::Ast::Kind::Bind);
  result &= Test(out, "Bind::classof()", bind->classof(bind.get()));

  result &= Test(out, "Bind::GetLoc()", bind_loc == bind->GetLoc());

  result &= Test(out, "Bind::symbol", bind->GetSymbol() == variable);
  result &= Test(out, "Bind::term", bind->GetAffix() == boolean_pointer);

  std::string bind_str =
      std::string(variable) + std::string(" := ") + boolean_pointer->ToString();

  result &= Test(out, "Bind::ToString()", bind->ToString() == bind_str);

  auto bind_type = bind->Typecheck(*env);

  result &=
      Test(out, "Bind::Typecheck()",
           bind_type && bind_type.GetFirst() == env->types->GetBoolType());

  result &= Test(out, "pink::Bind", result);
  out << "\n-----------------------\n";
  return result;
}
