
#include "ast/TestArray.h"
#include "ast/Array.h"
#include "ast/Integer.h"

#include "Test.h"

#include "aux/Environment.h"

auto TestArray(std::ostream &out) -> bool {
  bool result = true;
  out << "\n---------------------------------\n";
  out << "Testing pink::Array:\n";

  auto options = std::make_shared<pink::CLIOptions>();
  auto env = pink::Environment::NewGlobalEnv(options);

  // "[0, 1, 2, 3, 4];"
  // NOLINTBEGIN
  pink::Location integer_zero_loc(1, 2, 1, 3);
  pink::Location integer_one_loc(1, 5, 1, 6);
  pink::Location integer_two_loc(1, 8, 1, 9);
  pink::Location integer_three_loc(1, 11, 1, 12);
  pink::Location integer_four_loc(1, 14, 1, 15);
  pink::Location array_loc(1, 1, 1, 15);
  auto integer_zero = std::make_unique<pink::Integer>(integer_zero_loc, 0);
  auto integer_one = std::make_unique<pink::Integer>(integer_one_loc, 1);
  auto integer_two = std::make_unique<pink::Integer>(integer_two_loc, 2);
  auto integer_three = std::make_unique<pink::Integer>(integer_three_loc, 3);
  auto integer_four = std::make_unique<pink::Integer>(integer_four_loc, 4);
  // NOLINTEND

  std::vector<std::unique_ptr<pink::Ast>> array_elements;
  array_elements.emplace_back(std::move(integer_zero));
  array_elements.emplace_back(std::move(integer_one));
  array_elements.emplace_back(std::move(integer_two));
  array_elements.emplace_back(std::move(integer_three));
  array_elements.emplace_back(std::move(integer_four));

  auto array =
      std::make_unique<pink::Array>(array_loc, std::move(array_elements));

  result &=
      Test(out, "Array::GetKind()", array->GetKind() == pink::Ast::Kind::Array);
  result &= Test(out, "Array::classof()", array->classof(array.get()));
  result &= Test(out, "Array::GetLoc()", array->GetLoc() == array_loc);
  std::string array_str = "[0, 1, 2, 3, 4]";
  result &= Test(out, "Array::ToString()", array->ToString() == array_str);

  auto *integer_type = env->types->GetIntType();
  auto *array_type = env->types->GetArrayType(5, integer_type); // NOLINT

  auto typecheck_result = array->Typecheck(*env);
  result &= Test(out, "Array::Typecheck()",
                 typecheck_result && typecheck_result.GetFirst() == array_type);

  Test(out, "pink::Array", result);
  return result;
}
