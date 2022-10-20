#include "Test.h"

#include "type/BoolType.h"
#include "type/IntType.h"
#include "type/TupleType.h"

#include "type/TestTupleType.h"

auto TestTupleType(std::ostream &out) -> bool {
  bool result = true;
  out << "\n--------------------------------\n";
  out << "Testing pink::TupleType:\n";

  auto integer_type = std::make_unique<pink::IntType>();
  auto boolean_type = std::make_unique<pink::BoolType>();

  std::vector<pink::Type *> tuple_elements = {integer_type.get(),
                                              boolean_type.get()};

  auto tuple_type = std::make_unique<pink::TupleType>(tuple_elements);

  result &= Test(out, "TupleType::GetKind()",
                 tuple_type->GetKind() == pink::Type::Kind::Tuple);

  result &=
      Test(out, "TupleType::classof()", tuple_type->classof(tuple_type.get()));

  std::string tuple_string = "(Int, Bool)";
  result &= Test(out, "TupleType::ToString()",
                 tuple_type->ToString() == tuple_string);

  Test(out, "pink::TupleType", result);
  out << "\n--------------------------------\n";
  return result;
}