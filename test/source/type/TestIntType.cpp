#include "type/TestIntType.h"

#include "type/IntType.h"

#include "Test.h"

bool TestIntType(std::ostream &out) {
  bool        result = true;
  std::string name   = "pink::IntType";
  TestHeader(out, name);

  auto a = std::make_unique<pink::IntegerType>();

  result &= Test(out, "IntType::GetKind()",
                 a->GetKind() == pink::Type::Kind::Integer);

  result &= Test(out, "IntType::classof()", llvm::isa<pink::IntegerType>(a));

  return TestFooter(out, name, result);
}
