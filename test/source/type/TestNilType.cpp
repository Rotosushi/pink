#include "type/TestTypeAndNilType.h"

#include "type/NilType.h"

#include "Test.h"

bool TestTypeAndNilType(std::ostream &out) {
  bool result = true;
  out << "\n-----------------------\n";
  out << "Testing pink::Type and pink::NilType: \n";

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

      -) NilType::ToString() == "Nil"

  */

  pink::NilType a;

  result &=
      Test(out, "NilType::GetKind()", a.GetKind() == pink::Type::Kind::Nil);

  result &= Test(out, "NilType::classof()", a.classof(&a));

  std::string nil_type_str = "Nil";

  result &= Test(out, "NilType::ToString()", a.ToString() == nil_type_str);

  result &= Test(out, "pink::Type, pink::NilType", result);
  out << "\n-----------------------\n";
  return result;
}
