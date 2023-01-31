#include "type/TestBoolType.h"

#include "type/BoolType.h"

#include "Test.h"

bool TestBoolType(std::ostream &out) {
  bool        result = true;
  std::string name   = "pink::BoolType";
  TestHeader(out, name);

  auto a = std::make_unique<pink::BooleanType>();

  result &= Test(out, "BoolType::GetKind()",
                 a->GetKind() == pink::Type::Kind::Boolean);

  result &= Test(out, "BoolType::classf()", llvm::isa<pink::BooleanType>(a));

  return TestFooter(out, name, result);
}
