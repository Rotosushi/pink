#include <sstream>

#include "Test.h"
#include "ast/Dot.h"
#include "ast/Int.h"
#include "ast/TestDot.h"
#include "ast/Variable.h"

#include "aux/Environment.h"

auto TestDot(std::ostream &out) -> bool {
  bool result = true;
  out << "\n-----------------------------\n";
  out << "Testing pink::Dot: \n";

  auto options = std::make_shared<pink::CLIOptions>();
  std::stringstream stream;
  stream.str("var tuple := (false, 12, true);\n");
  auto env = pink::NewGlobalEnv(options, &stream);

  auto tuple = env->parser->Parse(*env);

  assert(tuple);

  auto tuple_typecheck_result = tuple.GetFirst()->Typecheck(*env);

  assert(tuple_typecheck_result);

  // 2: "tuple.1;"
  //     ^     ^^
  //     1     7 8
  //  NOLINTBEGIN
  pink::Location variable_loc = {2, 1, 1, 6};
  pink::Location integer_loc = {2, 7, 2, 8};
  pink::Location dot_loc = {2, 1, 2, 8};
  // NOLINTEND

  const auto *variable_symbol = env->symbols->Intern("tuple");
  auto variable =
      std::make_unique<pink::Variable>(variable_loc, variable_symbol);
  auto integer = std::make_unique<pink::Int>(integer_loc, 1);
  auto dot = std::make_unique<pink::Dot>(dot_loc, std::move(variable),
                                         std::move(integer));

  result &= Test(out, "Dot::GetKind()", dot->GetKind() == pink::Ast::Kind::Dot);

  /// \todo refactor unary operator "*" and "&" to their own Ast nodes,
  /// just as dot "." is represented with 'Ast::Dot'.
  /// the reason being is their extra invariants; as of now "*" and "&"
  /// are being implemented within Ast::Unop. This is (I think) confusing
  /// the implementation of Ast::Unop, where it is doing more than simply
  /// checking the invariants of 'normal' unops. This is reflected (I think)
  /// in the implementation of "*" and "&", their codegen functions,
  /// which both simply return what they were passed without modification.
  /// The entire funcionality of the operation is just in changing how we
  /// treat the pointer to some allocation which llvm returns to represent
  /// local/global variables.
  /// this is distinct from the overloads of pointer arithmetic which are
  /// provided by binop "+". These overloads actually have something to
  /// do when we generate code for them. However it is similar in that the
  /// implementation of binop is larger as a result of them, because of the
  /// extra invariants that pointer "+" places on the expression. (namely
  /// that we need to have an overload of the pointer arithmetic implementation
  /// for any given type we have some pointer to. And it is also possible to
  /// define such an overload for any given type we want to compute "+" for.)

  result &= Test(out, "Dot::classof()", dot->classof(dot.get()));

  result &= Test(out, "Dot::GetLoc()", dot->GetLoc() == dot_loc);

  std::string dot_str = std::string("tuple.1");
  result &= Test(out, "Dot::ToString()", dot->ToString() == dot_str);

  auto *integer_type = env->types->GetIntType();
  auto typecheck_result = dot->Typecheck(*env);
  result &=
      Test(out, "Dot::Typecheck()",
           typecheck_result && typecheck_result.GetFirst() == integer_type);

  return Test(out, "pink::Dot", result);
}