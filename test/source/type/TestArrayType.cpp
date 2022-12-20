#include "Test.h"

#include "type/ArrayType.h"

#include "type/IntType.h"
#include "type/TestArrayType.h"

auto TestArrayType(std::ostream &out) -> bool {
  bool result = true;
  out << "\n--------------------------------\n";
  out << "Testing pink::ArrayType:\n";

  auto integer_type = std::make_unique<pink::IntegerType>();

  auto array_type =
      std::make_unique<pink::ArrayType>(5, integer_type.get()); // NOLINT

  result &= Test(out, "ArrayType::GetKind()",
                 array_type->GetKind() == pink::Type::Kind::Array);

  result &=
      Test(out, "ArrayType::classof()", array_type->classof(array_type.get()));

  std::string array_string = "[Int x 5]";
  result &= Test(out, "ArrayType::ToString()",
                 array_type->ToString() == array_string);

  Test(out, "pink::ArrayType", result);
  out << "\n--------------------------------\n";
  return result;
}