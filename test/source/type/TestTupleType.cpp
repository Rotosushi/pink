#include "Test.h"

#include "type/BoolType.h"
#include "type/IntType.h"
#include "type/TupleType.h"

#include "type/TestTupleType.h"

auto TestTupleType(std::ostream &out) -> bool {
  bool        result = true;
  std::string name   = "pink::TupleType";
  TestHeader(out, name);

  auto integer_type = std::make_unique<pink::IntegerType>();
  auto boolean_type = std::make_unique<pink::BooleanType>();

  std::vector<pink::Type::Pointer> tuple_elements = {integer_type.get(),
                                                     boolean_type.get()};

  auto tuple_type = std::make_unique<pink::TupleType>(tuple_elements);

  result &= Test(out, "TupleType::GetKind()",
                 tuple_type->GetKind() == pink::Type::Kind::Tuple);

  result &=
      Test(out, "TupleType::classof()", llvm::isa<pink::TupleType>(tuple_type));

  return TestFooter(out, name, result);
}