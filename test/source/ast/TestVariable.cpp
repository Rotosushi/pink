#include "ast/TestVariable.h"
#include "Test.h"

#include "ast/Variable.h"
#include "ast/Nil.h"

#include "ast/action/ToString.h"
#include "ast/action/Typecheck.h"

#include "aux/Environment.h"

auto TestVariable(std::ostream &out) -> bool {
  bool        result = true;
  std::string name   = "pink::Variable";
  TestHeader(out, name);

  auto options = std::make_shared<pink::CLIOptions>();
  auto env     = pink::Environment::NewGlobalEnv(options);

  // "v;"
  pink::InternedString symbol_v = env->symbols->Intern("v");

  llvm::Value *boolean      = env->instruction_builder->getFalse();
  auto         boolean_type = env->types->GetBoolType();

  env->bindings->Bind(symbol_v, boolean_type, boolean);

  pink::Location     variable_loc(1, 1, 1, 2);
  pink::Ast::Pointer variable =
      std::make_unique<pink::Variable>(variable_loc, symbol_v);

  result &= Test(out, "Variable::GetKind()",
                 variable->GetKind() == pink::Ast::Kind::Variable);
  result &=
      Test(out, "Variable::classof()", llvm::isa<pink::Variable>(variable));
  result &=
      Test(out, "Variable::GetLoc()", variable->GetLocation() == variable_loc);
  result &= Test(out, "Variable::ToString()", ToString(variable) == symbol_v);

  auto typecheck_result = Typecheck(variable, *env);
  result &=
      Test(out, "Variable::Typecheck()",
           typecheck_result && typecheck_result.GetFirst() == boolean_type);

  return TestFooter(out, name, result);
}
