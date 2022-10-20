#include "Test.h"

#include "type/IntType.h"
#include "type/PointerType.h"

#include "type/TestPointerType.h"

auto TestPointerType(std::ostream &out) -> bool {
  bool result = true;
  out << "\n--------------------------------\n";
  out << "Testing pink::PointerType:\n";

  auto integer_type = std::make_unique<pink::IntType>();
  auto pointer_type = std::make_unique<pink::PointerType>(integer_type.get());

  result &= Test(out, "PointerType::GetKind()",
                 pointer_type->GetKind() == pink::Type::Kind::Pointer);

  result &= Test(out, "PointerType::classof()",
                 pointer_type->classof(pointer_type.get()));

  std::string pointer_string = "ptr Int";
  result &= Test(out, "PointerType::ToString()",
                 pointer_type->ToString() == pointer_string);

  Test(out, "pink::PointerType", result);
  out << "\n--------------------------------\n";
  return result;
}
