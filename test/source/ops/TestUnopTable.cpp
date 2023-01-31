
#include "ops/TestUnopTable.h"
#include "Test.h"
#include "ops/UnopTable.h"

#include "aux/Environment.h"

#include "type/IntType.h"

#include "support/DisableWarning.h"

// NOTE: 9/15/2022
// This function is a stub which is only defined for the sake of the
// test within this file, and is not expected to have -any- functionality.
// as such we do not use all of the parameters. since this is expected,
// we are choosing to disable this specific warning just for the function.
NOWARN(
    "-Wunused-parameter",
    llvm::Value *test_table_fn(llvm::Value             *term,
                               const pink::Environment &env) {
      return nullptr;
    })

bool TestUnopTable(std::ostream &out) {
  bool        result = true;
  std::string name   = "pink::UnopTable";
  TestHeader(out, name);

  auto options = std::make_shared<pink::CLIOptions>();
  auto env     = pink::Environment::NewGlobalEnv(options);

  pink::InternedString minus = env->operators->Intern("-");
  pink::Type::Pointer  ty    = env->types->GetIntType();
  pink::UnopTable      unop_table;

  auto pair = unop_table.Register(minus, ty, ty, test_table_fn);

  result &= Test(out, "UnopTable::Register", pair.first == minus);

  auto opt = unop_table.Lookup(minus);

  result &=
      Test(out, "UnopTable::Lookup", opt.has_value() && pair.first == minus);

  unop_table.Unregister(minus);

  auto opt1 = unop_table.Lookup(minus);

  result &= Test(out, "UnopTable::Unregister", !opt1.has_value());

  return TestFooter(out, name, result);
}
