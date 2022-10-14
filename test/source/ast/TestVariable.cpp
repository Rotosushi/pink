#include "ast/TestVariable.h"
#include "Test.h"

#include "ast/Variable.h"
#include "aux/Environment.h"

#include "ast/Nil.h"

auto TestVariable(std::ostream &out) -> bool {
  bool result = true;

  out << "\n-----------------------\n";
  out << "Testing pink::Variable: \n";

  auto options = std::make_shared<pink::CLIOptions>();
  auto env = pink::NewGlobalEnv(options);

  // "v;"
  pink::InternedString symbol_v = env->symbols->Intern("v");

  llvm::Value *boolean = env->instruction_builder->getFalse();
  pink::Type *boolean_type = env->types->GetBoolType();

  env->bindings->Bind(symbol_v, boolean_type, boolean);

  pink::Location variable_loc(1, 1, 1, 2);
  auto variable = std::make_unique<pink::Variable>(variable_loc, symbol_v);

  result &= Test(out, "Variable::GetKind()",
                 variable->GetKind() == pink::Ast::Kind::Variable);
  result &= Test(out, "Variable::classof()", variable->classof(variable.get()));
  result &= Test(out, "Variable::GetLoc()", variable->GetLoc() == variable_loc);
  result &= Test(out, "Variable::symbol", variable->GetSymbol() == symbol_v);
  result &= Test(out, "Variable::ToString()", variable->ToString() == symbol_v);

  auto typecheck_result = variable->Typecheck(*env);
  result &=
      Test(out, "Variable::Typecheck()",
           typecheck_result && typecheck_result.GetFirst() == boolean_type);

  result &= Test(out, "pink::Variable", result);
  out << "\n-----------------------\n";
  return result;
}
