
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
    pink::Outcome<llvm::Value *, pink::Error> test_table_fn(
        llvm::Value *term, const pink::Environment &env) {
      return {pink::Error()};
    })

bool TestUnopTable(std::ostream &out) {
  bool result = true;
  out << "\n-----------------------\n";
  out << "Testing pink::UnopTable: \n";

  auto options = std::make_shared<pink::CLIOptions>();
  auto env = pink::NewGlobalEnv(options);

  pink::InternedString minus = env->operators->Intern("-");
  pink::Type *ty = env->types->GetIntType();
  pink::UnopTable unop_table;

  auto pair = unop_table.Register(minus, ty, ty, test_table_fn);

  result &= Test(out, "UnopTable::Register", pair.first == minus);

  auto opt = unop_table.Lookup(minus);

  result &=
      Test(out, "UnopTable::Lookup", opt.has_value() && pair.first == minus);

  unop_table.Unregister(minus);

  auto opt1 = unop_table.Lookup(minus);

  result &= Test(out, "UnopTable::Unregister", !opt1.has_value());

  result &= Test(out, "pink::UnopTable", result);
  out << "\n-----------------------\n";
  return result;
}
