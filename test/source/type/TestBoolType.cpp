#include "type/TestBoolType.h"

#include "type/BoolType.h"

#include "Test.h"

bool TestBoolType(std::ostream &out) {
  bool result = true;
  out << "\n-----------------------\n";
  out << "Testing and pink::BoolType: \n";

  /*
      The Type class itself only provides a small
      amount of the functionality of any given
      syntax node.

      tests:

      -) An Type node constructed with a particular kind
          holds that particular kind.

      -) classof() meets specifications:
          https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html#the-contract-of-classof

      -) An Type node constructed with a particular Location
          holds that particular Location.

      -) BoolType::ToString() == "Bool"

  */

  pink::BooleanType a;

  result &=
      Test(out, "BoolType::GetKind()", a.GetKind() == pink::Type::Kind::Bool);

  result &= Test(out, "BoolType::classf()", a.classof(&a));

  std::string bool_type_str = "Bool";

  result &= Test(out, "BoolType::ToString()", a.ToString() == bool_type_str);

  result &= Test(out, "pink::BoolType", result);
  out << "\n-----------------------\n";
  return result;
}
