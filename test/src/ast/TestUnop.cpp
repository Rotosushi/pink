#include "Test.h"
#include "ast/TestUnop.h"
#include "ast/Unop.h"
#include "ast/Int.h"

#include "aux/Environment.h"

bool TestUnop(std::ostream& out)
{
  bool result = true;
  result = true;

  out << "\n-----------------------\n";
  out << "Testing pink::Unop: \n";

  auto options = std::make_shared<pink::CLIOptions>();
  auto env = pink::NewGlobalEnv(options);

  pink::InternedString minus = env->operators->Intern("-");
  pink::Location l0(0, 0, 0, 1);
  pink::Location l1(0, 2, 0, 3);
  pink::Location l3(0, 0, 0, 3);
  pink::Type* int_t = env->types->GetIntType();
  std::unique_ptr<pink::Int> i0 = std::make_unique<pink::Int>(l1, 1);
  pink::Ast* i0_p = i0.get();
  std::unique_ptr<pink::Unop> u0 = std::make_unique<pink::Unop>(l3, minus, std::move(i0));

  /*
  The Ast class itself only provides a small
  amount of the functionality of any given
  syntax node.

  tests:

  -) An Ast node constructed with a particular kind
      holds that particular kind.

  -) classof() meets specifications:
      https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html#the-contract-of-classof

  -) An Ast node constructed with a particular Location
      holds that particular Location.

  -) Unop::symbol == symbol;

  -) Unop::right != nullptr;

  -) Unop::ToString() == std::string(op) + i0->ToString();

  */
  result &= Test(out, "Unop::GetKind()", u0->getKind() == pink::Ast::Kind::Unop);

  result &= Test(out, "Unop::classof()", u0->classof(u0.get()));

  pink::Location ul(u0->GetLoc());
  result &= Test(out, "Unop::GetLoc()", ul == l3);

  result &= Test(out, "Unop::symbol", u0->op == minus);

  result &= Test(out, "Unop::term", u0->right.get() == i0_p);

  std::string unop_str = std::string(minus) + i0_p->ToString();

  result &= Test(out, "Unop::ToString()", u0->ToString() == unop_str);
  
  pink::Outcome<pink::Type*, pink::Error> unop_type = u0->Getype(*env);
  
  result &= Test(out, "Unop::Getype()", unop_type && unop_type.GetFirst() == int_t);

  result &= Test(out, "pink::Unop", result);
  out << "\n-----------------------\n";
  return result;
}
