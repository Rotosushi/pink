
#include "Test.h"

#include "type/TestVoidType.h"
#include "type/VoidType.h"

auto TestVoidType(std::ostream &out) -> bool {
  bool result = true;
  out << "\n--------------------------------\n";
  out << "Testing pink::VoidType:\n";

  auto void_type = std::make_unique<pink::VoidType>();

  result &= Test(out, "VoidType::GetKind()",
                 void_type->GetKind() == pink::Type::Kind::Void);

  result &=
      Test(out, "VoidType::classof()", void_type->classof(void_type.get()));

  std::string void_string = "Void";
  result &=
      Test(out, "VoidType::ToString()", void_type->ToString() == void_string);

  Test(out, "pink::VoidType", result);
  out << "\n--------------------------------\n";
  return result;
}