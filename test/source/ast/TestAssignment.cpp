#include <memory>

#include "Test.h"
#include "ast/Assignment.h"
#include "ast/Bool.h"
#include "ast/TestAssignment.h"
#include "ast/Variable.h"

#include "aux/Environment.h"

auto TestAssignment(std::ostream &out) -> bool {
  bool result = true;
  out << "\n-----------------------\n";
  out << "Testing pink::Assignment: \n";

  auto options = std::make_shared<pink::CLIOptions>();
  auto env = pink::Environment::NewGlobalEnv(options);

  // "z = true\n"
  pink::InternedString symb = env->symbols->Intern("z");
  pink::Location left_loc(1, 0, 1, 1);   // NOLINT
  pink::Location right_loc(1, 4, 1, 8);  // NOLINT
  pink::Location assign_loc(1, 0, 1, 8); // NOLINT
  auto variable = std::make_unique<pink::Variable>(left_loc, symb);
  pink::Ast *variable_pointer = variable.get();
  auto boolean = std::make_unique<pink::Bool>(right_loc, false);
  pink::Ast *boolean_pointer = boolean.get();

  llvm::Value *boolean_value = env->instruction_builder->getFalse();
  pink::Type *boolean_type = env->types->GetBoolType();

  env->bindings->Bind(symb, boolean_type, boolean_value);

  auto assignment = std::make_unique<pink::Assignment>(
      assign_loc, std::move(variable), std::move(boolean));

  result &= Test(out, "Assignment::getKind()",
                 assignment->GetKind() == pink::Ast::Kind::Assignment);

  result &=
      Test(out, "Assignment::classof()", assignment->classof(assignment.get()));

  result &=
      Test(out, "Assignment::GetLoc()", assignment->GetLoc() == assign_loc);

  result &=
      Test(out, "Assignment::left", assignment->GetLeft() == variable_pointer);

  result &=
      Test(out, "Assignment::right", assignment->GetRight() == boolean_pointer);

  std::string assignment_string =
      variable_pointer->ToString() + " = " + boolean_pointer->ToString();

  result &= Test(out, "Assignment::ToString()",
                 assignment->ToString() == assignment_string);

  auto typecheck_result = assignment->Typecheck(*env);

  result &=
      Test(out, "Assignment::Typecheck()",
           typecheck_result && typecheck_result.GetFirst() == boolean_type);

  result &= Test(out, "pink::Assignment", result);
  out << "\n-----------------------\n";
  return result;
}
