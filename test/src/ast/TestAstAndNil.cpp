#include <string>
#include <sstream>

#include "Test.h"
#include "ast/TestAstAndNil.h"
#include "ast/Nil.h"

#include "aux/Environment.h"

bool TestAstAndNil(std::ostream& out)
{
  bool result = true;
  out << "\n-----------------------\n";
  out << "Testing pink::Ast and pink::Nil: \n";

  auto options = std::make_shared<pink::CLIOptions>();
  auto env = pink::NewGlobalEnv(options);

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

      -) Nil::ToString() == "nil"

  */

	pink::Type* nil_t = env->types->GetNilType();
  pink::Location l0(0, 14, 0, 15);
  std::unique_ptr<pink::Nil> a = std::make_unique<pink::Nil>(l0);

  result &= Test(out, "Nil::GetKind()", a->getKind() == pink::Ast::Kind::Nil);
  result &= Test(out, "Nil::classof()", a->classof(a.get()));

  pink::Location l1(l0);

  result &= Test(out, "Nil::GetLoc()", l0 == l1);

  std::string nil_str = "nil";

  result &= Test(out, "Nil::ToString()", a->ToString() == nil_str);
  
  pink::Outcome<pink::Type*, pink::Error> nil_type = a->Getype(*env);
  
  result &= Test(out, "Nil::Getype()", nil_type && nil_type.GetFirst() == nil_t);

  result &= Test(out, "pink::Nil", result);
  out << "\n-----------------------\n";
  return result;
}
