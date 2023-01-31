#include "Test.h"

#include "type/IntType.h"
#include "type/PointerType.h"

#include "type/TestPointerType.h"

auto TestPointerType(std::ostream &out) -> bool {
  bool        result = true;
  std::string name   = "pink::PointerType";
  TestHeader(out, name);

  auto integer_type = std::make_unique<pink::IntegerType>();
  auto pointer_type = std::make_unique<pink::PointerType>(integer_type.get());

  result &= Test(out, "PointerType::GetKind()",
                 pointer_type->GetKind() == pink::Type::Kind::Pointer);

  result &= Test(out, "PointerType::classof()",
                 pointer_type->classof(pointer_type.get()));

  return TestFooter(out, name, result);
}
