#include <memory>

#include "Test.h"
#include "ast/Assignment.h"
#include "ast/Boolean.h"
#include "ast/TestAssignment.h"
#include "ast/Variable.h"

#include "ast/action/ToString.h"
#include "ast/action/Typecheck.h"

#include "aux/Environment.h"

auto TestAssignment(std::ostream &out) -> bool {
  bool        result = true;
  std::string name   = "pink::Assignment";
  TestHeader(out, name);

  auto options = std::make_shared<pink::CLIOptions>();
  auto env     = pink::Environment::NewGlobalEnv(options);

  // "z = true\n"
  pink::InternedString symb = env->symbols->Intern("z");
  pink::Location       left_loc(1, 0, 1, 1);   // NOLINT
  pink::Location       right_loc(1, 4, 1, 8);  // NOLINT
  pink::Location       assign_loc(1, 0, 1, 8); // NOLINT
  auto variable = std::make_unique<pink::Variable>(left_loc, symb);
  pink::Ast::Pointer boolean =
      std::make_unique<pink::Boolean>(right_loc, false);
  auto *boolean_type = env->types->GetBoolType();

  env->bindings->Bind(symb, boolean_type, nullptr);

  pink::Ast::Pointer assignment = std::make_unique<pink::Assignment>(
      assign_loc, std::move(variable), std::move(boolean));

  result &= Test(out, "Assignment::getKind()",
                 assignment->GetKind() == pink::Ast::Kind::Assignment);
  result &= Test(out, "Assignment::classof()",
                 llvm::isa<pink::Assignment>(assignment));
  result &= Test(out, "Assignment::GetLoc()",
                 assignment->GetLocation() == assign_loc);

  std::string assignment_string = "z = false";
  result                        &= Test(out, "Assignment::ToString()",
                                        ToString(assignment) == assignment_string);

  auto typecheck_result = Typecheck(assignment, *env);

  result &=
      Test(out, "Assignment::Typecheck()",
           typecheck_result && typecheck_result.GetFirst() == boolean_type);

  return TestFooter(out, name, result);
}
