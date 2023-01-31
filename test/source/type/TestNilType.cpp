#include "type/TestNilType.h"

#include "type/NilType.h"

#include "Test.h"

bool TestNilType(std::ostream &out) {
  bool        result = true;
  std::string name   = "pink::NilType";
  TestHeader(out, name);

  auto a = std::make_unique<pink::NilType>();

  result &=
      Test(out, "NilType::GetKind()", a->GetKind() == pink::Type::Kind::Nil);

  result &= Test(out, "NilType::classof()", llvm::isa<pink::NilType>(a));

  return TestFooter(out, name, result);
}
