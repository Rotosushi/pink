#include <memory>

#include "Test.h"
#include "type/BoolType.h"
#include "type/FunctionType.h"
#include "type/IntType.h"
#include "type/TestFunctionType.h"

auto TestFunctionType(std::ostream &out) -> bool {
  bool        result = true;
  std::string name   = "pink::FunctionType";
  TestHeader(out, name);

  auto integer_type = std::make_unique<pink::IntegerType>();
  auto boolean_type = std::make_unique<pink::BooleanType>();

  std::vector<pink::Type::Pointer> arg_tys0(
      {integer_type.get(), integer_type.get()});

  auto function_type =
      std::make_unique<pink::FunctionType>(boolean_type.get(), arg_tys0);

  result &= Test(out, "FunctionType::GetKind",
                 function_type->GetKind() == pink::Type::Kind::Function);

  result &= Test(out, "FunctionType::classof",
                 llvm::isa<pink::FunctionType>(function_type));

  return TestFooter(out, name, result);
}
