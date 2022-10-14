#include "ast/TestBinop.h"
#include "Test.h"

#include "ast/Binop.h"
#include "ast/Int.h"

#include "aux/Environment.h"

auto TestBinop(std::ostream &out) -> bool {
  bool result = true;

  out << "\n-----------------------\n";
  out << "Testing pink::Binop: \n";

  auto options = std::make_shared<pink::CLIOptions>();
  auto env = pink::NewGlobalEnv(options);

  // "1 + 1\n"
  pink::InternedString plus = env->operators->Intern("+");
  pink::Location integer_1_loc(1, 0, 1, 1); // NOLINT
  pink::Location integer_2_loc(1, 4, 1, 5); // NOLINT
  pink::Location binop_loc(1, 0, 1, 5);     // NOLINT
  pink::Type *integer_type = env->types->GetIntType();
  auto integer_1 = std::make_unique<pink::Int>(integer_1_loc, 1);
  pink::Ast *integer_1_pointer = integer_1.get();
  auto integer_2 = std::make_unique<pink::Int>(integer_2_loc, 1);
  pink::Ast *integer_2_pointer = integer_2.get();
  auto binop = std::make_unique<pink::Binop>(
      binop_loc, plus, std::move(integer_1), std::move(integer_2));

  result &=
      Test(out, "Binop::GetKind()", binop->GetKind() == pink::Ast::Kind::Binop);
  result &= Test(out, "Binop::classof()", binop->classof(binop.get()));
  result &= Test(out, "Binop::GetLoc()", binop_loc == binop->GetLoc());
  result &= Test(out, "Binop::symbol", binop->GetOp() == plus);

  result &= Test(out, "Binop::left, Binop::right",
                 binop->GetLeft() == integer_1_pointer &&
                     binop->GetRight() == integer_2_pointer);

  std::string binop_str = integer_1_pointer->ToString() + " " +
                          std::string(plus) + " " +
                          integer_2_pointer->ToString();

  result &= Test(out, "Binop::ToString()", binop->ToString() == binop_str);

  auto binop_type = binop->Typecheck(*env);
  result &= Test(out, "Binop::Typecheck()",
                 binop_type && binop_type.GetFirst() == integer_type);

  result &= Test(out, "pink::Binop", result);
  out << "\n-----------------------\n";
  return result;
}
