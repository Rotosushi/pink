#include "ast/TestFunction.h"
#include "Test.h"
#include "ast/Function.h"

#include "ast/Boolean.h"
#include "ast/Integer.h"
#include "ast/Variable.h"

#include "ast/action/ToString.h"
#include "ast/action/Typecheck.h"

#include "aux/Environment.h"

auto TestFunction(std::ostream &out) -> bool {
  bool        result = true;
  std::string name   = "pink::Function";
  TestHeader(out, name);

  auto options = std::make_shared<pink::CLIOptions>();
  auto env     = pink::Environment::NewGlobalEnv(options);

  /*
  fn first(x: Integer, y: Boolean)
  {
    x;
  }

  fn second(x: Integer, y: Boolean)
  {
    y;
  }
  */

  pink::InternedString symbol_x     = env->symbols->Intern("x");
  pink::InternedString symbol_y     = env->symbols->Intern("y");
  pink::InternedString fname        = env->symbols->Intern("first");
  pink::InternedString sname        = env->symbols->Intern("second");
  pink::Type          *integer_type = env->types->GetIntType();
  pink::Type          *boolean_type = env->types->GetBoolType();
  // NOLINTBEGIN
  pink::Location       first_loc      = {1, 0, 4, 1};
  pink::Location       variable_x_loc = {3, 3, 3, 4};
  pink::Location       second_loc     = {6, 0, 9, 1};
  pink::Location       variable_y_loc = {8, 3, 8, 4};
  // NOLINTEND

  auto variable_x = std::make_unique<pink::Variable>(variable_x_loc, symbol_x);
  auto variable_y = std::make_unique<pink::Variable>(variable_y_loc, symbol_y);

  std::vector<std::pair<pink::InternedString, pink::Type *>> fargs = {
      {symbol_x, integer_type},
      {symbol_y, boolean_type}};
  std::vector<std::pair<pink::InternedString, pink::Type *>> sargs = {
      {symbol_x, integer_type},
      {symbol_y, boolean_type}};

  auto *ftype =
      env->types->GetFunctionType(integer_type, {integer_type, boolean_type});
  auto *stype =
      env->types->GetFunctionType(boolean_type, {integer_type, boolean_type});

  pink::Ast::Pointer first = std::make_unique<pink::Function>(
      first_loc, fname, fargs, std::move(variable_x), env->bindings.get());

  pink::Ast::Pointer second = std::make_unique<pink::Function>(
      second_loc, sname, sargs, std::move(variable_y), env->bindings.get());

  result &= Test(out, "Function::getKind()",
                 first->GetKind() == pink::Ast::Kind::Function);

  result &= Test(out, "Function::GetLoc()", first->GetLocation() == first_loc);

  result &= Test(out, "Function::classof()", llvm::isa<pink::Function>(first));

  auto fty_res = Typecheck(first, *env);
  auto sty_res = Typecheck(second, *env);

  result &= Test(out, "Function::Typecheck()",
                 (fty_res) && (sty_res) && (fty_res.GetFirst() == ftype) &&
                     (sty_res.GetFirst() == stype));

  return TestFooter(out, name, result);
}
