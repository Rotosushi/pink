#include "ast/TestFunction.h"
#include "Test.h"
#include "ast/Function.h"

#include "ast/Bool.h"
#include "ast/Int.h"
#include "ast/Variable.h"

#include "aux/Environment.h"

bool TestFunction(std::ostream &out) {
  bool result = true;
  out << "\n-----------------------\n";
  out << "Testing Pink::Function: \n";

  auto options = std::make_shared<pink::CLIOptions>();
  auto env = pink::NewGlobalEnv(options);

  /*

  fn first(x: Int, y: Bool)
  {
    x;
  }

  fn second(x: Int, y: Bool)
  {
    y;
  }

  */

  pink::InternedString x = env->symbols->Intern("x"),
                       y = env->symbols->Intern("y");
  pink::InternedString fname = env->symbols->Intern("first"),
                       sname = env->symbols->Intern("second");
  pink::Type *int_t = env->types->GetIntType(),
             *bool_t = env->types->GetBoolType();
  pink::Location fl = {0, 0, 4, 34};
  pink::Location xl = {3, 30, 3, 31};
  pink::Location sl = {6, 36, 9, 70};
  pink::Location yl = {8, 64, 8, 65};

  std::unique_ptr<pink::Ast> xv = std::make_unique<pink::Variable>(xl, x);
  std::unique_ptr<pink::Ast> yv = std::make_unique<pink::Variable>(yl, y);

  std::vector<std::pair<pink::InternedString, pink::Type *>> fargs = {
      {x, int_t}, {y, bool_t}};
  std::vector<std::pair<pink::InternedString, pink::Type *>> sargs = {
      {x, int_t}, {y, bool_t}};

  pink::Type *ftype = env->types->GetFunctionType(int_t, {int_t, bool_t});
  pink::Type *stype = env->types->GetFunctionType(bool_t, {int_t, bool_t});

  std::unique_ptr<pink::Function> first = std::make_unique<pink::Function>(
      fl, fname, fargs, std::move(xv), env->bindings.get());

  std::unique_ptr<pink::Function> second = std::make_unique<pink::Function>(
      sl, sname, sargs, std::move(yv), env->bindings.get());

  result &= Test(out, "Function::getKind()",
                 first->GetKind() == pink::Ast::Kind::Function);

  result &= Test(out, "Function::GetLoc()", first->GetLoc() == fl);

  result &= Test(out, "Function::classof()", first->classof(first.get()));

  pink::Outcome<pink::Type *, pink::Error> fty_res = first->Typecheck(*env);
  pink::Outcome<pink::Type *, pink::Error> sty_res = second->Typecheck(*env);

  result &=
      Test(out, "Function::Typecheck()",
           (fty_res) && (sty_res) && (fty_res.GetFirst()->EqualTo(ftype)) &&
               (sty_res.GetFirst()->EqualTo(stype)));

  result &= Test(out, "pink::Function", result);
  out << "\n-----------------------\n";
  return result;
}
