#include "Test.h"

#include "type/ArrayType.h"

#include "type/IntType.h"
#include "type/TestArrayType.h"

auto TestArrayType(std::ostream &out) -> bool {
  bool        result = true;
  std::string name   = "pink::ArrayType";
  TestHeader(out, name);

  auto integer_type = std::make_unique<pink::IntegerType>();

  auto array_type =
      std::make_unique<pink::ArrayType>(5, integer_type.get()); // NOLINT

  result &= Test(out, "ArrayType::GetKind()",
                 array_type->GetKind() == pink::Type::Kind::Array);

  result &=
      Test(out, "ArrayType::classof()", llvm::isa<pink::ArrayType>(array_type));

  return TestFooter(out, name, result);
}