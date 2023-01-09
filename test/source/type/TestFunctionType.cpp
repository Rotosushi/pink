#include <memory>

#include "Test.h"
#include "type/BoolType.h"
#include "type/FunctionType.h"
#include "type/IntType.h"
#include "type/TestFunctionType.h"

auto TestFunctionType(std::ostream &out) -> bool {
  bool result = true;
  out << "\n-----------------------\n";
  out << "Testing pink::FunctionType: \n";

  auto integer_type = std::make_unique<pink::IntegerType>();
  auto boolean_type = std::make_unique<pink::BooleanType>();

  std::vector<pink::Type *> arg_tys0({integer_type.get(), integer_type.get()});

  auto function_type =
      std::make_unique<pink::FunctionType>(boolean_type.get(), arg_tys0);

  result &= Test(out, "FunctionType::GetKind",
                 function_type->GetKind() == pink::Type::Kind::Function);

  result &= Test(out, "FunctionType::classof",
                 function_type->classof(function_type.get()));

  std::string ft_str = std::string("Integer -> Integer -> Boolean");

  result &=
      Test(out, "FunctionType::ToString", function_type->ToString() == ft_str);

  result &= Test(out, "pink::FunctionType", result);

  out << "\n-----------------------\n";
  return result;
}
