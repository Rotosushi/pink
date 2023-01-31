
#include "Test.h"

#include "type/TestVoidType.h"
#include "type/VoidType.h"

auto TestVoidType(std::ostream &out) -> bool {
  bool        result = true;
  std::string name   = "pink::VoidType";
  TestHeader(out, name);

  auto void_type = std::make_unique<pink::VoidType>();

  result &= Test(out, "VoidType::GetKind()",
                 void_type->GetKind() == pink::Type::Kind::Void);

  result &=
      Test(out, "VoidType::classof()", llvm::isa<pink::VoidType>(void_type));

  return TestFooter(out, name, result);
}