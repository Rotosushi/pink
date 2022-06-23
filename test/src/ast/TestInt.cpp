#include "Test.h"
#include "ast/TestInt.h"

#include "ast/Int.h"

#include "aux/Environment.h"


bool TestInt(std::ostream& out)
{
  int result = true;
  result = true;

  out << "\n-----------------------\n";
  out << "Testing pink::Int: \n";

  auto options = std::make_shared<pink::CLIOptions>();
  auto env = pink::NewGlobalEnv(options); 

	pink::Type* int_t = env->types->GetIntType();
  pink::Location l0(0, 3, 0, 7);
  std::unique_ptr<pink::Int> i0 = std::make_unique<pink::Int>(l0, 42);

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

      -) Int::ToString() == "42"
  */
  result &= Test(out, "Int::GetKind()", i0->getKind() == pink::Ast::Kind::Int);

  result &= Test(out, "Int::classof()", i0->classof(i0.get()));

  pink::Location l1(l0);
  pink::Location il(i0->GetLoc());
  result &= Test(out, "Int::GetLoc()", il == l1);

  result &= Test(out, "Int::value", i0->value == 42);

  result &= Test(out, "Int::ToString()", i0->ToString() == "42");
  
  pink::Outcome<pink::Type*, pink::Error> int_type = i0->Getype(env);
  
  result &= Test(out, "Int::Getype()", int_type && int_type.GetOne() == int_t);
  
  result &= Test(out, "pink::Int", result);
  out << "\n-----------------------\n";
  return result;
}
