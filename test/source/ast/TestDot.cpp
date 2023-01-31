#include <sstream>

#include "Test.h"
#include "ast/Dot.h"
#include "ast/Integer.h"
#include "ast/TestDot.h"
#include "ast/Variable.h"

#include "ast/action/ToString.h"
#include "ast/action/Typecheck.h"

#include "aux/Environment.h"

auto TestDot(std::ostream &out) -> bool {
  bool        result = true;
  std::string name   = "pink::Dot";
  TestHeader(out, name);

  auto              options = std::make_shared<pink::CLIOptions>();
  std::stringstream stream;
  stream.str("tuple := (false, 12, true);\n");
  auto env = pink::Environment::NewGlobalEnv(options, &stream);

  auto tuple_result = env->parser->Parse(*env);
  assert(tuple_result.GetWhich());
  auto &tuple = tuple_result.GetFirst();

  auto tuple_typecheck_result = Typecheck(tuple, *env);

  assert(tuple_typecheck_result);

  // 2: "tuple.1;"
  //     ^     ^^
  //     1     7 8
  //  NOLINTBEGIN
  pink::Location variable_loc = {2, 1, 1, 6};
  pink::Location integer_loc  = {2, 7, 2, 8};
  pink::Location dot_loc      = {2, 1, 2, 8};
  // NOLINTEND

  const auto *variable_symbol = env->symbols->Intern("tuple");
  auto        variable =
      std::make_unique<pink::Variable>(variable_loc, variable_symbol);
  auto               integer = std::make_unique<pink::Integer>(integer_loc, 1);
  pink::Ast::Pointer dot     = std::make_unique<pink::Dot>(
      dot_loc, std::move(variable), std::move(integer));

  result &= Test(out, "Dot::GetKind()", dot->GetKind() == pink::Ast::Kind::Dot);

  result &= Test(out, "Dot::classof()", llvm::isa<pink::Dot>(dot));

  result &= Test(out, "Dot::GetLoc()", dot->GetLocation() == dot_loc);

  std::string dot_str = std::string("tuple.1");
  result              &= Test(out, "Dot::ToString()", ToString(dot) == dot_str);

  auto *integer_type     = env->types->GetIntType();
  auto  typecheck_result = Typecheck(dot, *env);
  result &=
      Test(out, "Dot::Typecheck()",
           typecheck_result && typecheck_result.GetFirst() == integer_type);

  return TestFooter(out, name, result);
}