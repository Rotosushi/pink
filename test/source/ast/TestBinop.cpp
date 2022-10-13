#include "ast/TestBinop.h"
#include "Test.h"

#include "ast/Binop.h"
#include "ast/Int.h"

#include "aux/Environment.h"

bool TestBinop(std::ostream &out) {
  bool result = true;
  result = true;

  out << "\n-----------------------\n";
  out << "Testing pink::Binop: \n";

  auto options = std::make_shared<pink::CLIOptions>();
  auto env = pink::NewGlobalEnv(options);

  pink::InternedString plus = env->operators->Intern("+");
  pink::Location l0(0, 0, 0, 1);
  pink::Location l1(0, 2, 0, 3);
  pink::Location l2(0, 4, 0, 5);
  pink::Location l3(0, 0, 0, 5);
  pink::Type *int_t = env->types->GetIntType();
  std::unique_ptr<pink::Int> i0 = std::make_unique<pink::Int>(l0, 1);
  pink::Ast *i0_p = i0.get();
  std::unique_ptr<pink::Int> i1 = std::make_unique<pink::Int>(l2, 1);
  pink::Ast *i1_p = i1.get();
  std::unique_ptr<pink::Binop> b0 =
      std::make_unique<pink::Binop>(l3, plus, std::move(i0), std::move(i1));

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

  -) Binop::symbol == symbol;

  -) Binop::term != nullptr;

  -) Binop::ToString() == l->ToString() + std::string(op) + r->ToString();

  */
  result &=
      Test(out, "Binop::GetKind()", b0->GetKind() == pink::Ast::Kind::Binop);

  result &= Test(out, "Binop::classof()", b0->classof(b0.get()));

  pink::Location bl(b0->GetLoc());

  result &= Test(out, "Binop::GetLoc()", bl == l3);

  result &= Test(out, "Binop::symbol", b0->op == plus);

  result &= Test(out, "Binop::left, Binop::right",
                 b0->left.get() == i0_p && b0->right.get() == i1_p);

  std::string binop_str =
      i0_p->ToString() + " " + std::string(plus) + " " + i1_p->ToString();

  result &= Test(out, "Binop::ToString()", b0->ToString() == binop_str);

  pink::Outcome<pink::Type *, pink::Error> binop_type = b0->Typecheck(*env);

  result &= Test(out, "Binop::Typecheck()",
                 binop_type && binop_type.GetFirst() == int_t);

  result &= Test(out, "pink::Binop", result);
  out << "\n-----------------------\n";
  return result;
}
