#include "type/TestIntType.h"

#include "type/IntType.h"

#include "Test.h"

bool TestIntType(std::ostream &out) {
  bool result = true;
  out << "\n-----------------------\n";
  out << "Testing and pink::IntType: \n";

  /*
      The Type class itself only provides a small
      amount of the functionality of any given
      syntax node.

      tests:

      -) An Type node constructed with a particular kind
          holds that particular kind.

      -) classof() meets specifications:
          https://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html#the-contract-of-classof

      -) A Type node constructed with a particular Location
          holds that particular Location.

      -) IntType::ToString() == "Integer"

  */

  pink::IntegerType a;

  result &=
      Test(out, "IntType::GetKind()", a.GetKind() == pink::Type::Kind::Integer);

  result &= Test(out, "IntType::classof()", a.classof(&a));

  std::string int_type_str = "Integer";

  result &= Test(out, "IntType::ToString()", a.ToString() == int_type_str);

  result &= Test(out, "pink::IntType", result);
  out << "\n-----------------------\n";
  return result;
}
